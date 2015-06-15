/*
DomoSynJson 

Domotic for home

sync with domoticz

Using COSA libraries : https://github.com/mikaelpatel/Cosa/

MCP1:00 -> rez, hall entree
MCP1:01 -> rez, toilette
MCP1:02 -> rez, salon1
MCP1:03 -> rez, salon2
MCP1:04 -> rez, salon petite lampe
MCP1:05 -> rez, salle a manger
MCP1:06 -> rez, salle a manger petite lampe
MCP1:07 -> rez, cuisine ilot central
MCP1:08 -> rez, cuisine plan de travail
MCP1:09 -> rez, buandrie
MCP1:10 -> rez, salon bureau
MCP1:11 -> exterieur sud
MCP1:12 -> exterieur porche
MCP1:13 -> exterieur rue
MCP1:14 -> exterieur ouest
MCP1:15 -> escalier etage


MCP2:00 -> cave nord
MCP2:01 -> cave sud
MCP2:02 -> cave milieu
MCP2:03 -> escalier cave
MCP2:04 -> etage, couloir
MCP2:05 -> etage, ch parent centrale
MCP2:06 -> etage, ch parent lit
MCP2:07 -> etage, ch enfant 1 centrale
MCP2:08 -> etage, ch enfant 1 lit
MCP2:09 -> etage, ch enfant 2 centrale
MCP2:10 -> etage, ch enfant 2 lit
MCP2:11 -> etage, bureau
MCP2:12 -> etage salle de bain 1
MCP2:13 -> etage salle de bain 2
MCP2:14 -> etage salle de douche
MCP2:15 -> etage wc

PIN00 -> rx
PIN01 -> tx
PIN02 -> 
PIN03 ->
PIN04 ->
PIN05 ->
PIN06 ->
PIN07 ->
PIN08 ->
PIN09 ->
PIN10 ->
PIN11 ->
PIN12 ->
PIN13 ->
PIN14 ->
PIN15 ->
PIN16 ->
PIN17 ->
PIN18 ->
PIN19 ->
PIN20 -> i2c
PIN21 -> i2c
PIN22 ->
PIN23 ->
PIN24 ->
PIN25 ->
PIN26 ->
PIN27 ->
PIN28 ->
PIN29 ->
PIN30 ->
PIN31 ->
PIN32 ->
PIN33 ->
PIN34 ->
PIN35 ->
PIN36 ->
PIN37 ->
PIN38 ->
PIN39 ->
PIN40 ->
PIN41 ->
PIN42 ->
PIN43 ->
PIN44 ->
PIN45 ->
PIN46 ->
PIN47 ->
PIN48 ->
PIN49 ->
PIN50 ->
PIN51 ->
PIN52 ->
PIN53 ->

*/
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

#include "Cosa/INET.hh"
#include <DNS.h>
#include <DHCP.h>
#include <HTTP.h>
#include <W5100.h>
#include "Cosa/String.hh"
#include "Cosa/Memory.h"
#include "MCP23017.h"

// EEPROM variable
//#define DEBUG1
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

// Sensor activation
//#define SENSOR_ON

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

void WebServer::on_request(IOStream& page, char* method, char* path, char* query)
{

#if defined(DEBUG1)
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
#if defined(DEBUG1)
  trace << PSTR("Request Execute : Pin :") << pin << PSTR("Cmd :")  << cmd << endl;
#endif
  CmdExecute(pin, cmd);
#if defined(DEBUG1)
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
  char buf[96] = {0};
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
            if (buf[12] == 'O') {
              value = 1;
#if defined(DEBUG1)
              trace << PSTR("Response Update = OK") << endl;
#endif
            }

          }
          for (int j = 0; j < 32; j++) buf[j] = 0;
          i = 0;

        }
      }
      break;

    case GETSTAT :
#if defined(DEBUG1)
      trace << PSTR("Response GETSTAT :") << endl;
#endif
      while ((res = m_sock->getchar()) >= 0) {
        buf[i] = res;
        i++;
        if (res == '\n')
        {
#if defined(DEBUG1)
          int o = 0;
          while (buf[o] != '\n' ) {
            trace << o << PSTR(":") << buf[o] << PSTR("; ") ;
            o++ ;
          }
          trace << endl ;
          sleep (1);
#endif
          if ( buf[10] == 'D' && buf[11] == 'a' && buf[12] == 't' && buf[13] == 'a' )
          {
            if (buf[20] == 'n') value = 1;
            else if (buf[20] == 'f') value = 0;
#if defined(DEBUG1)
            trace << PSTR("DATA :") << value << endl;
#endif
          }
          if ( buf[10] == 'i' && buf[11] == 'd' && buf[12] == 'x')
          {
            int j = 0;
            char idxChar[3];
            while (buf[18 + j] != 34 && j < 3) {
              idxChar[j] = buf[18 + j] ;
              j++;
            }
            idx = (int)String(idxChar).toInt();
#if defined(DEBUG1)
            trace << PSTR("Idx :") << idx << endl;
#endif
          }


          for (int j = 0; j < 96; j++) buf[j] = 0;
          i = 0;
        }
      }
#if defined(DEBUG1)
      trace << PSTR("Idx : ") << idx << PSTR(" Data : ") << value << endl;
#endif
      break;
  }
  etat = 1;
#if defined(DEBUG1)

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
    uint32_t relais;
    PushButton(Board::DigitalPin pin, Button::Mode mode, int idxtmp, int humman, uint32_t relaisNew) :
      Button(pin, mode),
      state(false)
    {

      idx = idxtmp;
      hummanPin = humman;
      relais = relaisNew;

    }
    virtual void on_change(uint8_t type)
    {
#if defined(DEBUG1)
      trace << PSTR("Button : on_change : hummanpin : ") << hummanPin << PSTR(" Internal Pin : ") << get_pin() << PSTR(" Relais value : ") << relais << endl ;
#endif
      UNUSED(type);
      
      if (state == false ) {

        for (int i = 0; i < 32; i++)
          
          if (bit_get(relais,i)) pinMCPUp(i);
      }
      else {
        for (int i = 0; i < 32; i++)
          if (bit_get(relais,i)) pinMCPDown(i);
      }
      state = !state;
      executeButton(hummanPin, state, idx);
    }
};




// Definition du webserver
WebServer server;

// Definition du web client
WebClient client;

// Extension I2C MCP23017

MCP23017 mcp1(0);
MCP23017 mcp2(1);

Event event;

// Define pointer button

PushButton *buttonList[50];

// Define button
PushButton buttonElec(Board::D22, Button::ON_FALLING_MODE, 7, 22, 0x0000FFFF);
PushButton buttonElec2(Board::D23, Button::ON_FALLING_MODE, 8, 23, 0x00005555);

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



  ASSERT(ethernet.begin(ip, subnet));

 
  // Start the server
  ASSERT(server.begin(ethernet.socket(Socket::TCP, PORT)));
#if defined(DEBUG1)
  trace << PSTR("End setup, Serve listening on ") << ip << PORT;
#endif
#if defined(DEBUG1)
  trace << PSTR("Wait 2 sec ethernet ") ;
  
#endif  
  sleep(2);

  // initialisation 
  initPin();
}


void loop()
{
  // Service incoming requests
#if defined(DEBUG2)
  trace << PSTR("Begin loop, Event queue size : ") << Event::queue.available() << endl;
#endif
  while (Event::queue.dequeue( &event ))
    event.dispatch();
#if defined(DEBUG2)
  trace << PSTR(".") ;
#endif
  server.run(20);
#if defined(DEBUG2)
  trace << PSTR(";") ;
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
      pinButtonUp(pin);
      break;
    case 4:
      pinButtonDown(pin);
      break;
    case 5:
      pinMCPUp(pin);
      break;
    case 6:
      pinMCPDown(pin);
      break;
    case 7:
      pinButton(pin);
      break; 
    default :
      break;
  }
}

// Initialise pin, change it to your configuration
void initPin()
{
 buttonElec.begin();
 buttonElec2.begin();
#if defined(DEBUG1)
  trace << PSTR("Udate domoticz with 0 value ") << endl ;
#endif
  client.begin(ethernet.socket(Socket::TCP));
  client.typeClient = UPDATE;
  sleep(5);
#if defined(DEBUG1)
  trace << ("Telerupteur Pins ") << endl ;
#endif
  client.get(formBufferOff(buttonElec.idx).c_str());
 
  trace << PSTR("Client 22 ") << endl;
  
  client.get(formBufferOff(buttonElec2.idx).c_str());
  trace << PSTR("Client 23 :") << endl;
 


  client.end();
  client.etat = -1;
  sleep(5);
  mcp1.begin();
  buttonList[22] = &buttonElec;
 #if defined(DEBUG1)
  trace << ("ButtonElec ") << buttonElec.relais << endl ;
  trace << ("ButtonElec2 ") << buttonElec2.relais << endl ;
#endif 
  buttonList[23] = &buttonElec2;

}

String formBufferOff(int idx)
{
  String Buffer = PSTR("http://192.168.1.101:8080/json.htm?type=command&param=udevice&idx=");
  Buffer += idx;
  Buffer += PSTR("&nvalue=0&svalue=");
  trace << Buffer << endl;
  return Buffer;
}
void pinUp(int pin)
{
  OutputPin((Board::DigitalPin) pgm_read_byte(&digital_pin_map[pin]), 1);
}
void pinDown(int pin)
{
  OutputPin((Board::DigitalPin) pgm_read_byte(&digital_pin_map[pin]), 0);
}
void pinButtonUp(int pin)
{

}
void pinButtonDown(int pin)
{

}

void pinMCPUp(int pin)
{
#if defined(DEBUG1)
  trace << PSTR( "MCPUP : ") << pin <<  endl;
#endif
  MCP23017 *mcp = NULL;
  if (pin >= 0 && pin < 16)
  {
    mcp = &mcp1;
    #if defined(DEBUG1)
    trace << PSTR( "MCP1 doing : ") << PSTR( "MCP GPIO Val : ") << mcp->readGPIOAB() << endl;
    #endif
  }
  else if (pin >= 16 && pin < 32)
  {
    mcp = &mcp2;
    pin = pin - 16;
  }
  else if ( mcp == NULL ) return;
  mcp->pinMode(pin, OUTPUT);
  mcp->digitalWrite(pin, HIGH);
   #if defined(DEBUG1)
  trace << PSTR( "MCP GPIO Val : ") << mcp->readGPIOAB() << endl;
  #endif

}
void pinMCPDown(int pin)
{
#if defined(DEBUG1)
  trace << PSTR( "MCPDOWN : ") << pin << endl;
#endif
  MCP23017 *mcp = NULL;
  if (pin >= 0 && pin < 16)
  {
    mcp = &mcp1;
  }
  else if (pin >= 16 && pin < 32)
  {
    mcp = &mcp2;
    pin = pin - 16;
  }
  else if ( mcp == NULL ) return;
  mcp->pinMode(pin, OUTPUT);
  mcp->digitalWrite(pin, LOW);
}
void executeButton(int pin, bool state, int idx)
{
#if defined(DEBUG1)
  trace << ("Buton:ExecuteButton: client begin ") << endl ;
#endif
  client.begin(ethernet.socket(Socket::TCP));
  client.typeClient = UPDATE;
  String Buffer = PSTR("http://192.168.1.101:8080/json.htm?type=command&param=udevice&idx=");
  Buffer += idx;
  
#if defined(DEBUG1)
      trace << PSTR("Buton: Idx ")<< idx << PSTR(" client get, state : ") << state << endl ;
#endif
      if (state) Buffer += PSTR("&nvalue=1&svalue=");
      else Buffer += PSTR("&nvalue=0&svalue=");

     
   trace << Buffer << endl;
  client.get(Buffer.c_str());
  client.end();
  client.etat = -1;
}
void pinButton(int humanPin)
{
  buttonList[humanPin]->on_change(0);
}




