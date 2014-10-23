#include "Cosa/EEPROM.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Board.hh"
#include "Cosa/Types.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Button.hh"
#include "Cosa/Pin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/Event.hh"
#include "Cosa/PWMPin.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/INET.hh"
#include "Cosa/INET/HTTP.hh"
#include "Cosa/String.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Driver/DHT.hh"
// EEPROM variable

#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
OutputPin sd(Board::D4, 1);
#endif
// Network configuration
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define IP 192,168,1,110
#define SUBNET 255,255,255,0
#define IPREMOTE 192,168,1,101
#define PORT 8888
static const uint8_t mac[6] __PROGMEM = { MAC };

// W5100 Ethernet Controller
W5100 ethernet(mac);


// HTML end of line
#define CRLF "\r\n"

// Global time for sensor timer

uint32_t time ;



// Example WebServer that responds with HTTP page with state of pins
class WebServer : public HTTP::Server {
  public:
    // Construct WebServer and initiate sequence counter
    WebServer() : m_nr(0) {}

    // Response member function
    virtual void on_request(IOStream& page, char* method, char* path, char* query);

  private:
    // Request sequence number
    uint16_t m_nr;
};

void
WebServer::on_request(IOStream& page, char* method, char* path, char* query)
{



  // Get client connection information; MAC, IP address and port
  INET::addr_t addr;
  get_client(addr);

  // Digital pin status; table with green/red blocks and pin number

  char pinChar[3];
  int i = 0;
  int j = 0;
  while (query[i] != ';')
  {
    pinChar[j] = query[i];
    j++;
    i++;
  }
  char pinCmd[3];
  i++;
  j = 0;
  while (query[i] != ';')
  {
    pinCmd[j] = query[i];
    j++;
    i++;
  }
  long pin = String(pinChar).toInt();
  long cmd = String(pinCmd).toInt();
  CmdExecute(pin, cmd);


}
// Definition du webserver
WebServer server;

// Definition classe button
class PushButton : public Button {
  public:
    bool state;
    int idx;
    PushButton(Board::DigitalPin pin, Button::Mode mode, int idxtmp) :
      Button(pin, mode),
      state(false)
    {

      idx = idxtmp;
    }
    virtual void on_change(uint8_t type)
    {
      UNUSED(type);
      state = !state;
      int pin = get_pin();
      executeButton(pin, state);
    }
};

class DS18B20E : public DS18B20 {
  public :
    int idx;
    DS18B20E(OWI *pin, const char *name = NULL, int idxtmp=0) :
      DS18B20(pin, name)
    {
      idx = idxtmp;
    }
};
// Defenition classe client web

class WebClient : public HTTP::Client {
  public:
    virtual void on_response(const char* hostname, const char* path);
};
void
WebClient::on_response(const char* hostname, const char* path)
{
  uint32_t start = Watchdog::millis();
  uint32_t count = 0L;
  char buf[129];
  int res;
  trace << PSTR("URL: http://") << (char*) hostname;
  if (*path) trace << '/' << (char*) path;
  trace << endl;
#if defined(PRINT_RESPONSE)
  while ((res = m_sock->read(buf, sizeof(buf) - 1)) >= 0) {
    if (res > 0) {
      buf[res] = 0;
      trace << buf;
      count += res;
    }
  }
#else
  while ((res = m_sock->read(buf, sizeof(buf) - 1)) >= 0) {
    if (res > 0) {
      trace << '.';
      count += res;
      if ((count & 0xfffL) == 0) trace << endl;
    }
  }
#endif
  if ((count & 0xfffL) != 0) trace << endl;
  trace << PSTR("Total (byte): ") << count << endl;
  trace << PSTR("Time (ms): ") << Watchdog::millis() - start << endl;
}
// Definition du web client
WebClient client;

// Sensors definitions

DS18B20E* temp[10] = { NULL};
OWI* owiCannal[10] = { NULL};
DHT22* dht22Dev[10] = { NULL};
bool requestDone = false;


void setup()
{
  // Initiate uart and trace output stream. And watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinWebServer: started"));
  Watchdog::begin();
  time = Watchdog::millis();
  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };

  // Initialise pin type

  initPin();

  ASSERT(ethernet.begin(ip, subnet));

  // Start the server
  ASSERT(server.begin(ethernet.socket(Socket::TCP, PORT)));

}

void loop()
{
  // Service incoming requests
  Event event;
  if (Event::queue.available() > 0 ) Event::queue.await(&event); //Event::queue.await(&event);
  event.dispatch();
  uint32_t time_tmp = Watchdog::millis();
  if (time_tmp - time > 5 * 60 * 1000 ) {
    sensorsConvert();
    requestDone = true;
    time = time_tmp;
  }
  if (time_tmp - time >  5 * 1000 && requestDone == true) {
    readSensors();
    requestDone == false;
  }
  server.run();

}

void CmdExecute(int pin, int cmd ) {

  switch (cmd) {
    case 1:
      pinUp(pin);
      break;
    case 2:
      pinDown(pin);
      break;
    case 3:
      pinTelerupteur(pin);
      break;
    default :
      break;
  }
}

// Initialise pin change it to your configuration
void initPin()
{


}
void pinUp(int pin)
{
  OutputPin(digital_pin_map[pin], 1);
}
void pinDown(int pin)
{
  OutputPin(digital_pin_map[pin], 0);
}
void pinTelerupteur(int pin)
{
  OutputPin OPin = OutputPin(digital_pin_map[pin], 1);
  delay(500);
  OPin.write(0);
}
void executeButton(int pin, bool state)
{
  client.begin(ethernet.socket(Socket::TCP));
  switch (pin)
  {
    case 10:
      if (state) client.get("http://192.168.1.101/json.htm?type=command&param=switchlight&idx=8&switchcmd=On&level=0&passcode=");
      else client.get("http://192.168.1.101/json.htm?type=command&param=switchlight&idx=8&switchcmd=Off&level=0&passcode=");
      break;

  }
  client.end();
}
void sensorsConvert()
{
  ds18b20convert();

}
void ds18b20convert()
{
  for (uint8_t i = 0; i < membersof(owiCannal); i++) {
    if (owiCannal[i] != NULL) DS18B20::convert_request(owiCannal[i], 12, true);
  }
}
void readSensors()
{

  for (uint8_t i = 0; i < membersof(temp); i++) {
    if (temp[i] != NULL) temp[i]->read_scratchpad();

  }
  client.begin(ethernet.socket(Socket::TCP));
  for (uint8_t i = 0; i < membersof(temp); i++) {
    if (temp[i] != NULL) {
      sendJsonTemp(temp[i]->idx, temp[i]->get_temperature ());
    }

  }
  client.end();
}
void sendJsonTemp(int idx, int16_t temperature)
{
  String command = "http://192.168.1.101json.htm?type=command&param=udevice&idx=";
  command += String(idx);
  command += "&nvalue=0&svalue=";
  command += String(temperature);
  client.get(command.c_str());
}

