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
  int i=0;
  int j=0;
  while(query[i] != ';')
  {
    pinChar[j] = query[i];
    j++;
    i++;
  }
  char pinCmd[3];
  i++;
  j=0;
  while(query[i] != ';')
  {
    pinCmd[j] = query[i];
    j++;
    i++;
  }
  long pin = String(pinChar).toInt();
  long cmd = String(pinCmd).toInt();
  CmdExecute(pin,cmd);
    
  
}
WebServer server;


void setup()
{
  // Initiate uart and trace output stream. And watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinWebServer: started"));
  Watchdog::begin();

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
  server.run();
  
}

void CmdExecute(int pin, int cmd ) {
 
  switch(cmd) {
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


