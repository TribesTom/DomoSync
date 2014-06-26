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
#include <stdlib.h> // for malloc and free

#define BAUD_RATE 115200

#define EEPROM_SIZE 26

#define ID_PREFIX "DomoSy-Arduino-"

#define COMMAND_START_CHAR 'U'
#define COMMAND_END_STRING "\r\n"

#define RESPONSE_START_CHAR '\t'
#define RESPONSE_END_STRING ":)"
#define MAX_MSG_SIZE 32

volatile unsigned long lastMicros;
unsigned long elapsedMicros;

// EEPROM variable
EEPROM eeprom;
char idGen[EEPROM_SIZE] EEMEM;

// client master for button interuption
int clientMaster=-1;

// Pin variable
Pin* pinTable[100];
AnalogPin* analogPinTabble[16];
char id[EEPROM_SIZE];

void* operator new(size_t size) { return malloc(size); }
void operator delete(void* ptr) { if (ptr) free(ptr); }

// Classe for button
class PushButton : public Button {
private:
  uint8_t m_count;
public:
  PushButton(Board::DigitalPin pin, Button::Mode mode) :
    Button(pin, mode),
    m_count(0)
  {
  }

  virtual void on_change(uint8_t type)
  {
    UNUSED(type);
    m_count += 1;
    int pin = get_pin();
    trace << RESPONSE_START_CHAR << clientMaster << pin << PSTR(RESPONSE_END_STRING);
  }
};


void setup() {

  
  // Initialisation variable
  RTC::begin();
  Watchdog::begin(16, Watchdog::push_timeout_events);
  lastMicros = RTC::micros();
  uart.begin(BAUD_RATE);
  trace.begin(&uart); //, PSTR("Cosa UART: started"));

  // Id initialisation
  getID(id);
  if (id[0] != 'D') {
    generateID();
    getID(id);
  }

}

void loop() {
  
  Event event;
 // Event::queue.await(&event);
  event.dispatch();
  // calculate the elapsed time since last circle
  unsigned long currentMicros = RTC::micros();
  elapsedMicros = currentMicros > lastMicros ? currentMicros - lastMicros : 4294967295 - lastMicros + currentMicros;
  lastMicros = currentMicros;

  // listen to incoming commands
  int len;
  while (( len = uart.peekchar('\n')) < 0) {
    Watchdog::delay(10);
  }
  int pos = 0;
  char z;
  // drop useless data
  while ( (z = uart.getchar()) != COMMAND_START_CHAR && pos < len)
  {
    pos++;
  }
  
  // extract complete command
  if (pos < len) {
    {
      int taille = 1;
      char cmd[10];
      cmd[0] = z;
      while ((z = uart.getchar()) != '\n')
     
      { 
        cmd[taille] = z;
        taille++;
      }
      processCommand(cmd, taille+1);
   
      
    }
  }
}

char* getID(char *id) {

  unsigned char x;
  // Read Memory
  for (int i = 0; i < EEPROM_SIZE; i ++) {
    eeprom.read(&x, &idGen[i], sizeof(char));
    if (x == 0xFF) {
      return id;
    }
    id[i] = x;
    
  }
  return id;
}

// generate ID and save it into EEPROM
void generateID() {

  // Stream buffer
  IOBuffer<32> buf;
  IOStream cout(&buf);

  // Generate Random Number
  srandom(AnalogPin::sample(Board::A0, Board::AVCC_REFERENCE));
  int part1 = (random() % (10000 - 1000 + 1)) + 1000;
  int part2 = (random() % (10000 - 1000 + 1)) + 1000;

  // Generate ID in buffer
  cout << PSTR(ID_PREFIX) << part1  << '-' << part2;
  
  // Wrire Buffer to memory
  char c;
  for (int i = 0; i < EEPROM_SIZE - 1; i ++) {
    c = buf.getchar();
    eeprom.write(&idGen[i], &c, sizeof(char)) ;
  }
  c = 0xff;
  eeprom.write(&idGen[EEPROM_SIZE - 1], &c, sizeof(char));

}

void processCommand(char *cmd, int len) {
  
  // Command processing
  if (len > 3) {
    int cmdId = cmd[1];
    switch (cmdId) {
      case 0x30:
        cmdGetID(cmd, len);
        break;
      case 0x31:
        cmdPinModeOutput(cmd, len);
        break;
      case 0x32:
        cmdPinModeInput(cmd, len);
        break;
      case 0x33:
        cmdSetPinUp(cmd, len);
        break;
      case 0x34:
        cmdSetPinDown(cmd, len);
        break;
      case 0x35:
        cmdGetPinStatus(cmd, len);
        break;
      case 0x36:
        cmdAnalogWrite(cmd, len);
        break;
      case 0x37:
        cmdAnalogRead(cmd, len);
        break;
      case 0x38:
        cmdAnalogReference(cmd, len);
        break;
      case 0x39:
        cmdSetMaster(cmd, len);
        break;
      case 0x40:
        cmdSetButton(cmd, len);
        break;
      case 0x41:
        cmdAnalogSet(cmd, len);
        break;
      case 0x50:
        cmdReadDHT11(cmd, len);
        break;
      case 0x51:
        cmdReadSR04(cmd, len);
        break;
    }
  }
}

// command to retrive the ID
// example: 55 30 0D 0A
void cmdGetID(char* cmd, int len) {
  if (len > 3) {
    uart.flush();
    trace << RESPONSE_START_CHAR ;
    for (int u = 0; u < EEPROM_SIZE - 1; u++) trace << id[u];
    trace << PSTR(RESPONSE_END_STRING);
  }
}

// command to set pin as output
// example: 55 31 09 0D 0A
void cmdPinModeOutput(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL) delete(pinTable[pin]); 
    OutputPin* tmp = new OutputPin((Board::DigitalPin)pin, 0);
    pinTable[pin] = tmp;
  }
}

// command to set pin as input
// example: 55 32 09 0D 0A
void cmdPinModeInput(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL) delete(pinTable[pin]); 
    InputPin* tmp= new InputPin((Board::DigitalPin)pin);
    pinTable[pin] = tmp;
  }
}

// command to set pin up
// example: 55 33 09 0D 0A
void cmdSetPinUp(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if(pinTable[pin] != NULL ) {
    OutputPin* tmp = (OutputPin*)pinTable[pin];
    tmp->write(1);
    }
  }
}

// command to set pin down
// example: 55 34 09 0D 0A
void cmdSetPinDown(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if(pinTable[pin] != NULL ) {
    OutputPin* tmp = (OutputPin*)pinTable[pin];
    tmp->write(0);
    }
  }
}

// command to read pin status
// example: 55 35 09 01 0D 0A
void cmdGetPinStatus(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    char clientId = cmd[3];
    if(pinTable[pin] != NULL ){ 
    InputPin* tmp = (InputPin*)pinTable[pin];
    trace << RESPONSE_START_CHAR << clientId << tmp->read() << PSTR(RESPONSE_END_STRING);
    }
  } 
}
// command to set PWM pin
// example: 55 41 09 0D 0A
void cmdAnalogSet(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL) delete(pinTable[pin]); 
    PWMPin* tmp= new PWMPin((Board::PWMPin)pin);
    pinTable[pin] = tmp;
    
  }
}



// command to write PWN value
// example: 55 36 09 70 0D 0A
void cmdAnalogWrite(char* cmd, int len) {
  if (len > 5) {
    int pin = (int)cmd[2];
    uint8_t value = (uint8_t)cmd[3];
    if(pinTable[pin] != NULL ){
    PWMPin* tmp = (PWMPin*)pinTable[pin];
    tmp->set(value);
    }
    
  }
}

// command to read analog value
// example: 55 37 04 01 0D 0A
void cmdAnalogRead(char* cmd, int len) {
  if (len > 5) {
    int pin = (int)cmd[2];
    char clientId = cmd[3];
    uint16_t value = AnalogPin::sample((Board::AnalogPin)pin, Board::AVCC_REFERENCE);
    trace << RESPONSE_START_CHAR << clientId << value  << PSTR(RESPONSE_END_STRING);
  }
}

// command to set analog reference
// example: 55 38 01 0D 0A
void cmdAnalogReference(char* cmd, int len) {
  if (len > 4) {
    int type = (int)cmd[2];
    switch (type) {
      case 0x00:
        // analogReference(5);
        break;
      case 0x01:
        //    analogReference(0);
        break;
    }
  }
}
// command set master to button response
// example : 55 39 01 0D 0A
void cmdSetMaster(char* cmd, int len) {
  if (len > 3) {
    clientMaster = (int) cmd[2];
  }
}
// command to define a button type pin
// command : 55 40 09 0D 0A
void cmdSetButton(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL) delete(pinTable[pin]);
    PushButton* tmp = new PushButton((Board::DigitalPin)pin,Button::ON_FALLING_MODE);
    pinTable[pin] = tmp;
    tmp->begin();
  }
}
 
// command to read DHT11 sensor
// example: 55 50 04 01 0D 0A
void cmdReadDHT11(char* cmd, int len) {


}

// command to read HC-SR04 sensor
// example: 55 51 04 05 01 0D 0A
void cmdReadSR04(char* cmd, int len) {

}
