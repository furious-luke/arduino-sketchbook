#include <Arduino.h>
#include <SPI.h>
#include <WiFly.h>

// #define NDEBUG
// #define NLOG
#include <debug.h>

SpiUartDevice spi_serial;

///
///
///
void
setup()
{
   Serial.begin( 9600 );
   spi_serial.begin( 9600 );
   Serial.println( "Ready" );
}

///
///
///
void
loop()
{
   while(spi_serial.available() > 0) {
    Serial.write(spi_serial.read());
  }

  if(Serial.available()) { // Outgoing data
    //spi_serial.print(Serial.read(), BYTE);
     spi_serial.write( Serial.read() );
  }
}
