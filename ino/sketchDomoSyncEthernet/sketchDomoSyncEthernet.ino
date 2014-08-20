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

#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
OutputPin sd(Board::D4, 1);
#endif
// Network configuration
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define IP 192,168,1,110
#define SUBNET 255,255,255,0
#define IPREMOTE 192,168,1,3
#define PORT 8888
static const uint8_t mac[6] __PROGMEM = { MAC };

// W5100 Ethernet Controller
W5100 ethernet(mac);
Socket* sock = NULL;
IOStream com;

// client master for button interuption
char clientMaster = -1;

// Pin variable
Pin* pinTable[100];
AnalogPin* analogPinTabble[16];
char id[EEPROM_SIZE];

void* operator new(size_t size) {
  return malloc(size);
}
void operator delete(void* ptr) {
  if (ptr) free(ptr);
}
// Daclare classes
class PushButton ;
class Node;
class Torch  ;

// Classe for manipulating dependencie between pin like interupter and light
class Node {
  public:
    Pin* pinPtr;
    Node* next;
    Node* prev;
    Node(Pin* pin)
    {
      pinPtr = pin;
    }
    ~Node()
    {

    }
};

// Classe torch to command telerupter
class Torch : public OutputPin {
  public:
    bool state;
    Node* ButtonList;
  
    Torch(Board::DigitalPin pin) :
      OutputPin(pin),
      state(false)
    {
      ButtonList = NULL;
    }
    ~Torch();
    void AddToList(PushButton* but);
    void RemoveFromList(PushButton* but);
} ;



// Classe for button to light lamp
class PushButton : public Button {
  public:
    bool state;
    Node* TorchList;

    PushButton(Board::DigitalPin pin, Button::Mode mode) :
      Button(pin, mode),
      state(false)
    {
      TorchList = NULL;
    }

    virtual void on_change(uint8_t type)
    {
      UNUSED(type);
      state = !state;
      int pin = get_pin();
      com << RESPONSE_START_CHAR << clientMaster << pin << PSTR(RESPONSE_END_STRING);
      Bascule(state);
    }
    bool get_state()
    {
      return state;
    }
    void Bascule(bool etat)
    {
      if (TorchList == NULL) return;
      int looping = 1;
      Node* ptr = TorchList;
      Torch* tmp;
      while (looping) {
        tmp = (Torch*)ptr->pinPtr;
        if(tmp->state != state ) tmp->write(1);
        if (ptr->next != NULL ) ptr = ptr->next;
        else looping = 0;
      }
      delay(150);
      ptr = TorchList;
      looping = 1;
      while (looping) {
        tmp = (Torch*)ptr->pinPtr;
        if(tmp->state != state ) tmp->write(0);
        tmp->state= state;
        if (ptr->next != NULL ) ptr = ptr->next;
        else looping = 0;
      }
    }
    void RemoveFromList(Torch* ptrSearch) {
      if (TorchList == NULL) return;
      Node* ptrList = TorchList;
      if ((Torch*)ptrList->pinPtr == ptrSearch) {
        if (ptrList->next == NULL) TorchList = NULL;
        else {
          TorchList = ptrList->next;
          TorchList->prev = NULL;
        }
        delete(ptrList);
        return;
      }
      while (ptrList->next != NULL) {
        ptrList = ptrList->next;
        if ((Torch*)ptrList->pinPtr == ptrSearch) {
          if (ptrList->next == NULL) ptrList->prev = NULL;
          else {
            ptrList->prev->next = ptrList->next;
            ptrList->next->prev = ptrList->prev;
          }

          delete(ptrList);
          return;
        }

      }
    }
    void AddToList(Torch* tor)
    {
      if (tor == NULL) return;
      Node*tmp = new Node((Pin*)tor);
      if (TorchList == NULL) TorchList = tmp;
      else {
        Node* ptrList = TorchList;
        while (ptrList->next != NULL) ptrList = ptrList->next;
        ptrList->next = tmp;
        tmp->prev = ptrList;
      }


    }
};

Torch::~Torch() {

  if (ButtonList != NULL) {
    Node* UsePtr;

    int Looping = 1;
    while (Looping) {
      UsePtr = ButtonList;
      PushButton* ButPtr;
      if (ButtonList->next == NULL) Looping = 0;
      else ButtonList = ButtonList->next;
      ButPtr = (PushButton*)UsePtr->pinPtr;
      ButPtr->RemoveFromList(this);
      delete(UsePtr);
    }
  }
};
void Torch::RemoveFromList(PushButton* ptrSearch) {
  if (ButtonList == NULL) return;
  Node* ptrList = ButtonList;
  if ((Button*)ptrList->pinPtr == ptrSearch) {
    if (ptrList->next == NULL) ButtonList = NULL;
    else {
      ButtonList = ptrList->next;
      ButtonList->prev = NULL;
    }
    delete(ptrList);
    return;
  }
  while (ptrList->next != NULL) {
    ptrList = ptrList->next;
    if ((PushButton*)ptrList->pinPtr == ptrSearch) {
      if (ptrList->next == NULL) ptrList->prev = NULL;
      else {
        ptrList->prev->next = ptrList->next;
        ptrList->next->prev = ptrList->prev;
      }

      delete(ptrList);
      return;
    }
  }
};
void Torch::AddToList(PushButton* but)
{
  if (but == NULL) return;
  Node*tmp = new Node((Pin*)but);
  if (ButtonList == NULL) ButtonList = tmp;
  else {
    Node* ptrList = ButtonList;
    while (ptrList->next != NULL) ptrList = ptrList->next;
    ptrList->next = tmp;
    tmp->prev = ptrList;
  }
};






void setup() {


  // Initialisation variable
  RTC::begin();
  Watchdog::begin(16, Watchdog::push_timeout_events);
  lastMicros = RTC::micros();
  uart.begin(BAUD_RATE);
  trace.begin(&uart); //, PSTR("Cosa UART: started"));
  trace << PSTR("Get ID") << endl;
  // Id initialisation
  getID(id);
  if (id[0] != 'D') {
    generateID();
    getID(id);
  }
  sleep(1);
  // Initialise Ethernet shield
  trace << PSTR("Try Socket") << endl;
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  bool result = ethernet.begin(ip, subnet) ;
  if ( result == false) trace << PSTR("Ethernet begin failed") << endl;
  sleep(1);
  sock = openConnection();

}

void loop() {

  Event event;
  if (Event::queue.available() > 0 ) Event::queue.await(&event); //Event::queue.await(&event);
  event.dispatch();
  // calculate the elapsed time since last circle
  unsigned long currentMicros = RTC::micros();
  elapsedMicros = currentMicros > lastMicros ? currentMicros - lastMicros : 4294967295 - lastMicros + currentMicros;
  lastMicros = currentMicros;

  // listen to incoming commands

  if (sock == NULL )
  {
    openConnection();
    return ;
  }
  if (sock->isconnected() <= 0) {
    sock->disconnect();
    sock->close();
    sock = openConnection();
    return;
  }
  //trace << PSTR("Wait command") << endl;
  int pos = 0;
  char z = 'z';
  char cmd[10];
  int len = 0;
  //trace << PSTR("Info sock : ") << sock << endl;
  if ( (len = sock->available()) > 0 ) {
    trace << PSTR("Bytes Available :") << len << endl;
    while ( z != COMMAND_START_CHAR && pos < len )
    {
      sock->read(&z, sizeof(char));
      pos++;
    }
    trace << PSTR("Process Command : ") ;
    int taille = 0;
    if (pos < len ) {


      while ( z != '\n' &&  taille < MAX_MSG_SIZE)
      {

        trace << taille << PSTR(":") << (int)z;
        cmd[taille] = z;
        sock->read(&z, sizeof(char));
        taille++;
      }
    }

    trace << PSTR("end") << endl;
    if ( pos < MAX_MSG_SIZE ) processCommand(cmd, taille + 1);
  }
}

Socket* openConnection()
{
  trace << PSTR("Open Connection") << endl;
  Socket* sock_tmp = NULL;
  sock_tmp = ethernet.socket(Socket::TCP);
  trace << PSTR("Opened Connection") << endl;
  sleep(1);
  if (sock_tmp == NULL) {
    trace << PSTR("Socket NULL") << endl;
    return NULL;
  }
  uint8_t ipremote1[4] = { IPREMOTE };
  uint16_t port1 = PORT;
  trace << PSTR("Try connect") << ipremote1[0] << ipremote1[1] << ipremote1[2] << ipremote1[3] << PSTR(":") << PORT << endl;
  sleep(1);
  int res = sock_tmp->connect(ipremote1, port1);
  trace << PSTR("Socket trying done ") << endl ;
  sleep(1);
  if ( res >= 0)
  {
    trace << PSTR("Socket Opened ") << endl ;
    com.set_device(sock_tmp);
    trace << PSTR("Com begin on sock : ") << sock_tmp << endl ;
    return sock_tmp;
  }
  else {
    trace << PSTR("Failed Socket");
    return NULL;
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

  trace << PSTR("Execute Command :" ) << (int)cmd[1] << PSTR("Taille buf : ") << len << endl;
  // Command processing
  if (len > 2) {
    int cmdId = cmd[1];
    switch (cmdId) {
      case 0x30:
        cmdGetID(cmd, len);
        break;
      case 0x31:
        cmdPinModeOutput(cmd, len);
        break;
      case 0x32:
        cmdSetPinUp(cmd, len);
        break;
      case 0x33:
        cmdSetPinDown(cmd, len);
        break;
      case 0x34:
        cmdPinModeInput(cmd, len);
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
        cmdSetButtonTorch(cmd, len);
        break;
      case 0x42:
        cmdRemButtonTorch(cmd, len);
        break;
      case 0x50:
        cmdReadDHT11(cmd, len);
        break;
      case 0x51:
        cmdReadSR04(cmd, len);
        break;
      case 0x60:
        cmdSetTorch(cmd, len);
        break;

    }
  }
}

// command to retrive the ID
// example: 55 30 0D 0A
void cmdGetID(char* cmd, int len) {
  if (len > 3) {
    uart.flush();
    com << RESPONSE_START_CHAR ;
    for (int u = 0; u < EEPROM_SIZE - 1; u++) com << id[u];
    //sock->write(id,EEPROM_SIZE);
    com << PSTR(RESPONSE_END_STRING) << flush;
    trace << PSTR("Send ID") << endl;
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
    InputPin* tmp = new InputPin((Board::DigitalPin)pin);
    pinTable[pin] = tmp;
  }
}

// command to set pin up
// example: 55 33 09 0D 0A
void cmdSetPinUp(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL ) {
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
    if (pinTable[pin] != NULL ) {
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
    if (pinTable[pin] != NULL ) {
      InputPin* tmp = (InputPin*)pinTable[pin];
      com << RESPONSE_START_CHAR << clientId << tmp->read() << PSTR(RESPONSE_END_STRING) << flush;
    }
  }
}
// command to set PWM pin
// example: 55 41 09 0D 0A
void cmdAnalogSet(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL) delete(pinTable[pin]);
    PWMPin* tmp = new PWMPin((Board::PWMPin)pin);
    pinTable[pin] = tmp;

  }
}



// command to write PWN value
// example: 55 36 09 70 0D 0A
void cmdAnalogWrite(char* cmd, int len) {
  if (len > 5) {
    int pin = (int)cmd[2];
    uint8_t value = (uint8_t)cmd[3];
    if (pinTable[pin] != NULL ) {
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
    com << RESPONSE_START_CHAR << clientId << value  << PSTR(RESPONSE_END_STRING) << flush;
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
// example : 55 39 22 01 0D 0A
void cmdSetMaster(char* cmd, int len) {
  if (len > 3) {
    clientMaster = cmd[3];
  }
}
// command to define a button type pin
// command : 55 40 09 0D 0A
void cmdSetButton(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL) delete(pinTable[pin]);
    PushButton* tmp = new PushButton((Board::DigitalPin)pin, Button::ON_FALLING_MODE);
    pinTable[pin] = tmp;
    tmp->begin();
  }
}
// command to add Torch tu PushButton type pin
// command : 55 41 17 09 0D 0A
void cmdSetButtonTorch(char* cmd, int len) {
  if (len > 5) {
    int pin = (int)cmd[2];
    int pinT = (int)cmd[3];
    if (pinTable[pin] == NULL || pinTable[pinT]==NULL) return;
    PushButton* tmpBut =(PushButton*) pinTable[pin];
    tmpBut->AddToList((Torch*)pinTable[pinT]);
    Torch* tmpT = (Torch*) pinTable[pinT];
    tmpT->AddToList((PushButton*)pinTable[pin]);
    
  }
}
// command to remove Torch from PushButton
// command : 55 42 17 09 0D 0A
void cmdRemButtonTorch(char* cmd, int len) {
  if (len > 5) {
    int pin = (int)cmd[2];
    int pinT = (int)cmd[3];
    if (pinTable[pin] == NULL || pinTable[pinT]==NULL) return;
    PushButton* tmpBut =(PushButton*) pinTable[pin];
    tmpBut->RemoveFromList((Torch*)pinTable[pinT]);
    Torch* tmpT = (Torch*) pinTable[pinT];
    tmpT->RemoveFromList((PushButton*)pinTable[pin]);
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
// command to define a Torch pin
// command : 55 60 09 0D 0A
void cmdSetTorch(char* cmd, int len) {
  if (len > 4) {
    int pin = (int)cmd[2];
    if (pinTable[pin] != NULL) delete(pinTable[pin]);
    Torch* tmp = new Torch((Board::DigitalPin)pin);
    pinTable[pin] = tmp;
  }
}
