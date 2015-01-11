/*************************************************** 
 This is a library for the MCP23017 i2c port expander

 These displays use I2C to communicate, 2 pins are required to
 interface
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, all text above must be included in any redistribution
 ***************************************************
 Updated by TribesTom
 
 Changed tu support COSA arduino libraries.
 
 
 
 */
#include "Cosa/TWI.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
#define MCP23017_ADDRESS 0x20

// registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14


#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

#define MCP23017_INT_ERR 255

#define INPUT 1
#define OUTPUT 0

#define CHANGE 0

#define FALLING 1
#define RISING 0

#define HIGH 1
#define LOW 0


class MCP23017 : private TWI::Driver {
public:
  MCP23017(uint8_t addr) :
  TWI::Driver(MCP23017_ADDRESS| (addr & 0x7))
  {}
  void begin();
  void pinMode(uint8_t p, uint8_t d);
  void digitalWrite(uint8_t p, uint8_t d);
  void pullUp(uint8_t p, uint8_t d);
  uint8_t digitalRead(uint8_t p);

  void writeGPIOAB(uint16_t);
  uint16_t readGPIOAB();
  uint8_t readGPIO(uint8_t b);

  void setupInterrupts(uint8_t mirroring, uint8_t open, uint8_t polarity);
  void setupInterruptPin(uint8_t p, uint8_t mode);
  uint8_t getLastInterruptPin();
  uint8_t getLastInterruptPinValue();

protected:
/** Pin number mask. */
  static const uint8_t PIN_MASK = 0x07;

/* Construct connection to MCP23017 Remote 16-bit I/O expander with
* given address.
* @param[in] addr bus address.
* @param[in] subaddr device sub address.
*/
  MCP23017(uint8_t addr, uint8_t subaddr) :
  TWI::Driver(addr | (subaddr & 0x7))
  {}

private:
  uint8_t bitForPin(uint8_t pin);
  uint8_t regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr);

  uint8_t readRegister(uint8_t addr);
  void writeRegister(uint8_t addr, uint8_t value);

  /**
   * Utility private method to update a register associated with a pin (whether port A/B)
   * reads its value, updates the particular bit, and writes its value.
   */
  void updateRegisterBit(uint8_t p, uint8_t pValue, uint8_t portAaddr, uint8_t portBaddr);

};
 



/**
 * Bit number associated to a give Pin
 */
uint8_t MCP23017::bitForPin(uint8_t pin){
	return pin%8;
}

/**
 * Register address, port dependent, for a given PIN
 */
uint8_t MCP23017::regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr){
	return(pin<8) ?portAaddr:portBaddr;
}

/**
 * Reads a given register
 */
uint8_t MCP23017::readRegister(uint8_t addr){
	// read the current GPINTEN
	uint8_t res;
	twi.begin(this);
	twi.write(addr);
	twi.read(&res,sizeof(res));
	twi.end();
        trace << PSTR("ReadRegister addr : ") << addr << PSTR(" ReadRegister Res value : ") << res << endl;
        return res;

}


/**
 * Writes a given register
 */
void MCP23017::writeRegister(uint8_t regAddr, uint8_t regValue){
	// Write the register
	twi.begin(this);
	twi.write(regAddr,&regValue,sizeof(regAddr));
        twi.end();
        trace << PSTR("WriteRegister regAddr : ") << regAddr << PSTR(" WriteRegister reg value : ") << regValue << endl;
}


/**
 * Helper to update a single bit of an A/B register.
 * - Reads the current register value
 * - Writes the new register value
 */
void MCP23017::updateRegisterBit(uint8_t pin, uint8_t pValue, uint8_t portAaddr, uint8_t portBaddr) {
	uint8_t regValue;
	uint8_t regAddr=regForPin(pin,portAaddr,portBaddr);
	uint8_t bit=bitForPin(pin);
	regValue = readRegister(regAddr);

	// set the value for the particular bit
	bit_write(pValue,regValue,bit);

	writeRegister(regAddr,regValue);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the MCP23017 given its HW selected address, see datasheet for Address selection.
 */
void MCP23017::begin() {
	// set defaults!
	// all inputs on port A and B
        readRegister(MCP23017_IODIRA);
        readRegister(MCP23017_IODIRB);
	writeRegister(MCP23017_IODIRA,0xff);
	writeRegister(MCP23017_IODIRB,0xff);
        readRegister(MCP23017_IODIRA);
        readRegister(MCP23017_IODIRB);
        readRegister(MCP23017_IOCONA);
        readRegister(MCP23017_IOCONB);
	
}

/**
 * Sets the pin mode to either INPUT or OUTPUT
 */
void MCP23017::pinMode(uint8_t p, uint8_t d) {
	updateRegisterBit(p,(d==INPUT),MCP23017_IODIRA,MCP23017_IODIRB);
}

/**
 * Reads all 16 pins (port A and B) into a single 16 bits variable.
 */
uint16_t MCP23017::readGPIOAB() {
	uint16_t ba = 0;
	

	// read the current GPIO output latches
	twi.begin(this);
	twi.write((uint8_t)MCP23017_GPIOA);
	

	twi.read(&ba, sizeof(ba));
	twi.end();
        
	return ba;
}

/**
 * Read a single port, A or B, and return its current 8 bit value.
 * Parameter b should be 0 for GPIOA, and 1 for GPIOB.
 */
uint8_t MCP23017::readGPIO(uint8_t b) {

	// read the current GPIO output latches
	uint8_t res;
	
	twi.begin(this);
	if (b == 0)
              {
		twi.write((uint8_t)MCP23017_GPIOA);                
              }
	else {
		twi.write((uint8_t)MCP23017_GPIOB);                
	}
	

	twi.read(&res,sizeof(res));
        
	return res;
}

/**
 * Writes all the pins in one go. This method is very useful if you are implementing a multiplexed matrix and want to get a decent refresh rate.
 */
void MCP23017::writeGPIOAB(uint16_t ba) {
	twi.begin(this);
	twi.write((uint8_t)MCP23017_GPIOA,&ba,sizeof(ba));
	twi.end();
}

void MCP23017::digitalWrite(uint8_t pin, uint8_t d) {
	uint8_t gpio;
	uint8_t bit=bitForPin(pin);


	// read the current GPIO output latches
	uint8_t regAddr=regForPin(pin,MCP23017_OLATA,MCP23017_OLATB);
	//uint8_t regAddr=regForPin(pin,MCP23017_GPIOA,MCP23017_GPIOB);
        gpio = readRegister(regAddr);

	// set the pin and direction
	bit_write(d,gpio,bit);

	// write the new GPIO
	regAddr=regForPin(pin,MCP23017_GPIOA,MCP23017_GPIOB);
	writeRegister(regAddr,gpio);
}

void MCP23017::pullUp(uint8_t p, uint8_t d) {
	updateRegisterBit(p,d,MCP23017_GPPUA,MCP23017_GPPUB);
}

uint8_t MCP23017::digitalRead(uint8_t pin) {
	uint8_t bit=bitForPin(pin);
	uint8_t regAddr=regForPin(pin,MCP23017_GPIOA,MCP23017_GPIOB);
	return (readRegister(regAddr) >> bit) & 0x1;
}

/**
 * Configures the interrupt system. both port A and B are assigned the same configuration.
 * Mirroring will OR both INTA and INTB pins.
 * Opendrain will set the INT pin to value or open drain.
 * polarity will set LOW or HIGH on interrupt.
 * Default values after Power On Reset are: (false,flase, LOW)
 * If you are connecting the INTA/B pin to arduino 2/3, you should configure the interupt handling as FALLING with
 * the default configuration.
 */
void MCP23017::setupInterrupts(uint8_t mirroring, uint8_t openDrain, uint8_t polarity){
	// configure the port A
	uint8_t ioconfValue=readRegister(MCP23017_IOCONA);
	bit_write(mirroring,ioconfValue,6);
	bit_write(openDrain,ioconfValue,2);
	bit_write(polarity,ioconfValue,1);
	writeRegister(MCP23017_IOCONA,ioconfValue);

	// Configure the port B
	ioconfValue=readRegister(MCP23017_IOCONB);
	bit_write(mirroring,ioconfValue,6);
	bit_write(openDrain,ioconfValue,2);
	bit_write(polarity,ioconfValue,1);
	writeRegister(MCP23017_IOCONB,ioconfValue);
}

/**
 * Set's up a pin for interrupt. uses arduino MODEs: CHANGE, FALLING, RISING.
 *
 * Note that the interrupt condition finishes when you read the information about the port / value
 * that caused the interrupt or you read the port itself. Check the datasheet can be confusing.
 *
 */
void MCP23017::setupInterruptPin(uint8_t pin, uint8_t mode) {

	// set the pin interrupt control (0 means change, 1 means compare against given value);
	updateRegisterBit(pin,(mode!=CHANGE),MCP23017_INTCONA,MCP23017_INTCONB);
	// if the mode is not CHANGE, we need to set up a default value, different value triggers interrupt

	// In a RISING interrupt the default value is 0, interrupt is triggered when the pin goes to 1.
	// In a FALLING interrupt the default value is 1, interrupt is triggered when pin goes to 0.
	updateRegisterBit(pin,(mode==FALLING),MCP23017_DEFVALA,MCP23017_DEFVALB);

	// enable the pin for interrupt
	updateRegisterBit(pin,HIGH,MCP23017_GPINTENA,MCP23017_GPINTENB);

}

uint8_t MCP23017::getLastInterruptPin(){
	uint8_t intf;

	// try port A
	intf=readRegister(MCP23017_INTFA);
	for(int i=0;i<8;i++) if (bit_get(intf,i)) return i;

	// try port B
	intf=readRegister(MCP23017_INTFB);
	for(int i=0;i<8;i++) if (bit_get(intf,i)) return i+8;

	return MCP23017_INT_ERR;

}
uint8_t MCP23017::getLastInterruptPinValue(){
	uint8_t intPin=getLastInterruptPin();
	if(intPin!=MCP23017_INT_ERR){
		uint8_t intcapreg=regForPin(intPin,MCP23017_INTCAPA,MCP23017_INTCAPB);
		uint8_t bit=bitForPin(intPin);
		return (readRegister(intcapreg)>>bit) & (0x01);
	}

	return MCP23017_INT_ERR;
}
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
