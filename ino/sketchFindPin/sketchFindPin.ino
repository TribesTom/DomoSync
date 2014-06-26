#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"



void setup()
{
  // Start the UART and Watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaUART: started (LINE MODE)"));
  Watchdog::begin();
}

void loop()
{
  trace << PSTR("Pin D0 :") << Board::D0 << endl;
  trace << PSTR("Pin D1 :") << Board::D1 << endl;
  trace << PSTR("Pin D2 :") << Board::D2 << endl;
  trace << PSTR("Pin D3 :") << Board::D3 << endl;
  trace << PSTR("Pin D4 :") << Board::D4 << endl;
  trace << PSTR("Pin D5 :") << Board::D5 << endl;
  trace << PSTR("Pin D6 :") << Board::D6 << endl;
  trace << PSTR("Pin D7 :") << Board::D7 << endl;
  trace << PSTR("Pin D8 :") << Board::D8 << endl;
  trace << PSTR("Pin D9 :") << Board::D9 << endl;
  
  trace << PSTR("Pin D10 :") << Board::D10 << endl;
  trace << PSTR("Pin D11 :") << Board::D11 << endl;
  trace << PSTR("Pin D12 :") << Board::D12 << endl;
  trace << PSTR("Pin D13 :") << Board::D13 << endl;
  trace << PSTR("Pin D14 :") << Board::D14 << endl;
  trace << PSTR("Pin D15 :") << Board::D15 << endl;
  trace << PSTR("Pin D16 :") << Board::D16 << endl;
  trace << PSTR("Pin D17 :") << Board::D17 << endl;
  trace << PSTR("Pin D18 :") << Board::D18 << endl;
  trace << PSTR("Pin D19 :") << Board::D19 << endl;
  
  trace << PSTR("Pin D20 :") << Board::D20 << endl;
  trace << PSTR("Pin D21 :") << Board::D21 << endl;
  trace << PSTR("Pin D22 :") << Board::D22 << endl;
  trace << PSTR("Pin D23 :") << Board::D23 << endl;
  trace << PSTR("Pin D24 :") << Board::D24 << endl;
  trace << PSTR("Pin D25 :") << Board::D25 << endl;
  trace << PSTR("Pin D26 :") << Board::D26 << endl;
  trace << PSTR("Pin D27 :") << Board::D27 << endl;
  trace << PSTR("Pin D28 :") << Board::D28 << endl;
  trace << PSTR("Pin D29 :") << Board::D29 << endl;
  
  trace << PSTR("Pin D30 :") << Board::D30 << endl;
  trace << PSTR("Pin D31 :") << Board::D31 << endl;
  trace << PSTR("Pin D32 :") << Board::D32 << endl;
  trace << PSTR("Pin D33 :") << Board::D33 << endl;
  trace << PSTR("Pin D34 :") << Board::D34 << endl;
  trace << PSTR("Pin D35 :") << Board::D35 << endl;
  trace << PSTR("Pin D36 :") << Board::D36 << endl;
  trace << PSTR("Pin D37 :") << Board::D37 << endl;
  trace << PSTR("Pin D38 :") << Board::D38 << endl;
  trace << PSTR("Pin D39 :") << Board::D39 << endl;
  
  trace << PSTR("Pin D40 :") << Board::D40 << endl;
  trace << PSTR("Pin D41 :") << Board::D41 << endl;
  trace << PSTR("Pin D42 :") << Board::D42 << endl;
  trace << PSTR("Pin D43 :") << Board::D43 << endl;
  trace << PSTR("Pin D44 :") << Board::D44 << endl;
  trace << PSTR("Pin D45 :") << Board::D45 << endl;
  trace << PSTR("Pin D46 :") << Board::D46 << endl;
  trace << PSTR("Pin D47 :") << Board::D47 << endl;
  trace << PSTR("Pin D48 :") << Board::D48 << endl;
  trace << PSTR("Pin D49 :") << Board::D49 << endl;
  
  trace << PSTR("Pin D50 :") << Board::D50 << endl;
  trace << PSTR("Pin D51 :") << Board::D51 << endl;
  trace << PSTR("Pin D52 :") << Board::D52 << endl;
  trace << PSTR("Pin D53 :") << Board::D53 << endl;
  
  ASSERT(true == false);
}
