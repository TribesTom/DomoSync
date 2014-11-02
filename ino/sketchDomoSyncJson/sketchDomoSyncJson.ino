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
#include "Cosa/Memory.h"

// EEPROM variable
#define DEBUG
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

// Client web type

#define UPDATE 0
#define GETSTAT 1

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

#if defined(DEBUG)
  trace << PSTR("Request Here : ") << path << query << endl ;
#endif
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
#if defined(DEBUG)
  trace << PSTR("Request Execute : Pin :") << pin << PSTR("Cmd :")  << cmd << endl;
#endif
  CmdExecute(pin, cmd);
#if defined(DEBUG)
  trace << PSTR("Free memory : ") << free_memory() << endl;
#endif

}

// Defenition classe client web

class WebClient : public HTTP::Client {
  public:
    int typeClient = UPDATE;
    int value = 0;
    int etat = -1;
    int idx = 0;
    virtual void on_response(const char* hostname, const char* path);
};

void WebClient::on_response(const char* hostname, const char* path)
{
  uint32_t start = Watchdog::millis();
  uint32_t count = 0L;
  char buf[32] = {0};
  char res;
  int i = 0;
  etat = 0 ;
  switch (typeClient)
  {
    case UPDATE :

      while ((res = m_sock->getchar()) >= 0) {
        buf[i] = res;
        if (res == '\n')
        {
          if ( String(buf).startsWith("\"status\""))
          {
            if (buf[12] == 'O') value = 1;

          }
          for (int j = 0; j < 32; j++) buf[j] = 0;
          i = 0;

        }
      }
      break;

    case GETSTAT :

      while ((res = m_sock->getchar()) >= 0) {
        buf[i] = res;
        if (res == '\n')
        {
          if ( String(buf).startsWith("\"Data\""))
          {
            if (buf[11] == 'n') value = 1;
            if (buf[11] == 'f') value = 0;
          }
          if ( String(buf).startsWith("\"Idx\""))
          {
            int j = 0;
            char idxChar[3];
            while (buf[i + j] != 34) idxChar[j] = buf[i + j] ;
            idx = (int)String(idxChar).toInt();
          }


          for (int j = 0; j < 32; j++) buf[j] = 0;
          i = 0;

        }
      }
#if defined(DEBUG)
      trace << PSTR("Idx : ") << idx << PSTR(" Data : ") << value << endl;
#endif
      break;
  }
  etat = 1;
#if defined(DEBUG)

  trace << PSTR("Total (byte): ") << count << endl;
  trace << PSTR("Time (ms): ") << Watchdog::millis() - start << endl;
#endif
}


// Definition classe button
class PushButton : public Button {
  public:
    bool state;
    int idx;
    int hummanPin;
    PushButton(Board::DigitalPin pin, Button::Mode mode, int idxtmp, int humman) :
      Button(pin, mode),
      state(false)
    {

      idx = idxtmp;
      hummanPin = humman;

    }
    virtual void on_change(uint8_t type)
    {
#if defined(DEBUG)
      trace << PSTR("Button : on_change : hummanpin : ") << hummanPin << PSTR(" Internal Pin : ") << get_pin() << endl ;
#endif
      UNUSED(type);
      state = !state;
      executeButton(hummanPin, state);
    }
};

class DS18B20E : public DS18B20 {
  public :
    int idx;
    DS18B20E(OWI *pin, const char *name = NULL, int idxtmp = 0) :
      DS18B20(pin, name)
    {
      idx = idxtmp;
    }
};


// Definition du webserver
WebServer server;

// Definition du web client
WebClient client;

// Sensors definitions

DS18B20E* temp[10] = { NULL};
OWI* owiCannal[10] = { NULL};
DHT22* dht22Dev[10] = { NULL};
bool requestDone = false;


// Define button

PushButton buttonElec(Board::D22, Button::ON_FALLING_MODE, 8, 22);

void setup()
{
  // Initiate uart and trace output stream. And watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinWebServer: started"));
  //Watchdog::begin();
  Watchdog::begin(16, Watchdog::push_timeout_events);
  time = Watchdog::millis();
  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };

  // Initialise pin type

  initPin();

  ASSERT(ethernet.begin(ip, subnet));

  // Start the server
  ASSERT(server.begin(ethernet.socket(Socket::TCP, PORT)));
#if defined(DEBUG)
  trace << PSTR("End setup, Serve listening on ") << ip << PORT;
#endif
}


void loop()
{
  // Service incoming requests
#if defined(DEBUG)
  trace << PSTR("Begin loop, Event queue size : ") << Event::queue.available() << endl;
#endif
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
#if defined(DEBUG)
  trace << PSTR("Begin run") << endl;
#endif
  server.run(1000);
#if defined(DEBUG)
  trace << PSTR("End run") << endl;
#endif
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

  buttonElec.begin();

}
void pinUp(int pin)
{
  OutputPin((Board::DigitalPin) pgm_read_byte(&digital_pin_map[pin]), 1);
}
void pinDown(int pin)
{
  OutputPin((Board::DigitalPin) pgm_read_byte(&digital_pin_map[pin]), 0);
}
void pinTelerupteur(int pin)
{
#if defined(DEBUG)
  trace << PSTR( "Pin Telerupteur Pin : ") << pin << PSTR(" digital pin map : ") << (Board::DigitalPin) pgm_read_byte(&digital_pin_map[pin]) << endl;
#endif
  OutputPin OPin = OutputPin((Board::DigitalPin) pgm_read_byte(&digital_pin_map[pin]), 0);
  delay(500);
  OPin.write(1);
}
void executeButton(int pin, bool state)
{
#if defined(DEBUG)
  trace << ("Buton:ExecuteButton: client begin ") << endl ;
#endif
  client.begin(ethernet.socket(Socket::TCP));
  client.typeClient = UPDATE;
  switch (pin)
  {
    case 22:
#if defined(DEBUG)
      trace << ("Buton: Case 22 : client get, state : ") << state << endl ;
#endif
      if (state) client.get("http://192.168.1.101:8080/json.htm?type=command&param=switchlight&idx=8&switchcmd=On&level=0&passcode=");
      else client.get("http://192.168.1.101:8080/json.htm?type=command&param=switchlight&idx=8&switchcmd=Off&level=0&passcode=");
      break;

  }
  client.end();
  client.etat = -1;
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
  client.etat = -1;
}
void sendJsonTemp(int idx, int16_t temperature)
{
  String command = "http://192.168.1.101json.htm?type=command&param=udevice&idx=";
  command += String(idx);
  command += "&nvalue=0&svalue=";
  command += String(temperature);
  client.get(command.c_str());
}

