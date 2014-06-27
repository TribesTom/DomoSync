 int pinMap[] = {0,1,4,5,85,3,11,12,13,14,20,21,22,23,73,72,9,8,43,42,41,40,24,25,26,27,28,29,30,31,39,38,37,36,35,34,33,32,47,82,81,80,55,54,53,52,51,50,49,48,19,18,17,16};
 
 enum DigitalPin {
    D0 = 0,	// PE0
    D1 = 1,	// PE1
    D2 = 4,	// PE4
    D3 = 5,	// PE5
    D4 = 85,	// PG5
    D5 = 3,	// PE3
    D6 = 11,	// PH3
    D7 = 12,	// PH4
    D8 = 13,	// PH5
    D9 = 14,	// PH6
    D10 = 20,	// PB4
    D11 = 21,	// PB5
    D12 = 22,	// PB6
    D13 = 23,	// PB7
    D14 = 73,	// PJ1
    D15 = 72,	// PJ0
    D16 = 9,	// PH1
    D17 = 8,	// PH0
    D18 = 43,	// PD3
    D19 = 42,	// PD2
    D20 = 41,	// PD1
    D21 = 40,	// PD0
    D22 = 24,	// PA0
    D23 = 25,	// PA1
    D24 = 26,	// PA2
    D25 = 27,	// PA3
    D26 = 28,	// PA4
    D27 = 29,	// PA5
    D28 = 30,	// PA6
    D29 = 31,	// PA7
    D30 = 39,	// PC7
    D31 = 38,	// PC6
    D32 = 37,	// PC5
    D33 = 36,	// PC4
    D34 = 35,	// PC3
    D35 = 34,	// PC2
    D36 = 33,	// PC1
    D37 = 32,	// PC0
    D38 = 47,	// PD7
    D39 = 82,	// PG2
    D40 = 81,	// PG1
    D41 = 80,	// PG0
    D42 = 55,	// PL7
    D43 = 54,	// PL6
    D44 = 53,	// PL5
    D45 = 52,	// PL4
    D46 = 51,	// PL3
    D47 = 50,	// PL2
    D48 = 49,	// PL1
    D49 = 48,	// PL0
    D50 = 19,	// PB3/MISO
    D51 = 18,	// PB2/MOSI
    D52 = 17,	// PB1/SCK
    D53 = 16,	// PB0/SS
    LED = D13
  } ;

  /**
* Analog pin symbols (ADC channel numbers)
*/
  enum AnalogPin {
    A0 = 0,	// PF0
    A1 = 1,	// PF1
    A2 = 2,	// PF2
    A3 = 3,	// PF3
    A4 = 4,	// PF4
    A5 = 5,	// PF5
    A6 = 6,	// PF6
    A7 = 7,	// PF7
    A8 = 32,	// PK0
    A9 = 33,	// PK1
    A10 = 34,	// PK2
    A11 = 35,	// PK3
    A12 = 36,	// PK4
    A13 = 37,	// PK5
    A14 = 38,	// PK6
    A15 = 39	// PK7
  } ;

  enum PWMPin {
    PWM0 = D2,	// PE4 => OCR3B
    PWM1 = D3,	// PE5 => 0CR3C
    PWM2 = D4,	// PG5 => OCR0B
    PWM3 = D5,	// PE3 => OCR3A
    PWM4 = D6,	// PH3 => OCR4A
    PWM5 = D7,	// PH4 => OCR4B
    PWM6 = D8,	// PH5 => OCR4C
    PWM7 = D9,	// PH6 => OCR2B
    PWM8 = D10,	// PB4 => OCR2A
    PWM9 = D11,	// PB5 => OCR1A
    PWM10 = D12,	// PB6 => OCR1B
    PWM11 = D13	// PB7 => OCR0A
  } ;

  /**
* External interrupt pin symbols; sub-set of digital pins
* to allow compile time checking.
*/
  enum ExternalInterruptPin {
    EXT0 = D21,	// PD0
    EXT1 = D20,	// PD1
    EXT2 = D19,	// PD2
    EXT3 = D18,	// PD3
    EXT4 = D2,	// PE4
    EXT5 = D3	// PE5
  } ;

  /**
* Pin change interrupt. Number of port registers.
* Arduino Mega does not allow access to all pins (PCI8..15).
*/
  enum InterruptPin {
    PCI0 = D53,	// PB0
    PCI1 = D52,	// PB1
    PCI2 = D51,	// PB2
    PCI3 = D50,	// PB3
    PCI4 = D10,	// PB4
    PCI5 = D11,	// PB5
    PCI6 = D12,	// PB6
    PCI7 = D13,	// PB7
    PCI16 = A8,	// PK0
    PCI17 = A9,	// PK1
    PCI18 = A10,	// PK2
    PCI19 = A11,	// PK3
    PCI20 = A12,	// PK4
    PCI21 = A13,	// PK5
    PCI22 = A14,	// PK6
    PCI23 = A15	// PK7
  } ;

  /**
* Pins used for TWI interface (port D, D20-D21)
*/
  enum TWIPin {
    SCL = 0,	// PD0/D21
    SDA = 1	// PD1/D20
  };

  /**
* Pins used for SPI interface (port B, D50-D53)
*/
  enum SPIPin {
    SS = 0,	// PB0/D53
    SCK = 1,	// PB1/D52
    MOSI = 2,	// PB2/D51
    MISO = 3	// PB3/D50
  } ;

