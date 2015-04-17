
 // Created by TribesTom
 

#include "Cosa/TWI.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
#include "MCP23017.h"
MCP23017 mcp(0);
void setup() {
  // put your setup code here, to run once:
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinWebServer: started"));
  //Watchdog::begin();
  Watchdog::begin(16, Watchdog::push_timeout_events);
 trace << PSTR("Begin Output : ") << endl;
 mcp.begin();
 mcp.pinMode(8,OUTPUT);
 mcp.pinMode(9,OUTPUT);
 mcp.pinMode(10,OUTPUT);
 mcp.pinMode(11,OUTPUT);
 mcp.pinMode(12,OUTPUT);
 mcp.pinMode(13,OUTPUT);
 mcp.pinMode(14,OUTPUT);
 mcp.pinMode(15,OUTPUT);
 trace << PSTR("End Output : ") << endl;
      
}

void loop() {
  // put your main code here, to run repeatedly:
  trace << PSTR("Out 8 : ") << endl;
  mcp.digitalWrite(8,1);
  sleep(5);
  mcp.digitalWrite(8,0);
  sleep(5);
  trace << PSTR("Out 9 : ") << endl;
  mcp.digitalWrite(9,1);
  sleep(5);
  mcp.digitalWrite(9,0);
  sleep(5);
  trace << PSTR("Out 10 : ") << endl;
  mcp.digitalWrite(10,1);
  sleep(5);
  mcp.digitalWrite(10,0);
  sleep(5);
  trace << PSTR("Out 11 : ") << endl;
  mcp.digitalWrite(11,1);
  sleep(5);
  mcp.digitalWrite(11,0);
  sleep(5);
  trace << PSTR("Out 12 : ") << endl;
  mcp.digitalWrite(12,1);
  sleep(5);
  mcp.digitalWrite(12,0);
  sleep(5);
  trace << PSTR("Out 13 : ") << endl;
  mcp.digitalWrite(13,1);
  sleep(5);
  mcp.digitalWrite(13,0);
  sleep(5);
  trace << PSTR("Out 14 : ") << endl;
  mcp.digitalWrite(14,1);
  sleep(5);
  mcp.digitalWrite(14,0);
  sleep(5);
  trace << PSTR("Out 15 : ") << endl;
  mcp.digitalWrite(15,1);
  sleep(5);
  mcp.digitalWrite(15,0);
  sleep(5);
}
