#include <OneWire.h>
#include <DallasTemperature.h>

#define DELAY_MS     2000
#define N_SAMPLES    10
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire one_wire( ONE_WIRE_BUS );

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors( &one_wire );

float samples[N_SAMPLES];
float avg;
int pos;
int filled;

void
setup()
{
   // start serial port
   Serial.begin( 9600 );

   // Start up the library
   sensors.begin();

   pos = 0;
   filled = 0;
   avg = 0.0;
}

void
loop()
{
   delay( DELAY_MS );

   sensors.requestTemperatures();
   float temp = sensors.getTempCByIndex( 0 );

   if( filled == N_SAMPLES )
   {
      Serial.println( avg/(float)N_SAMPLES );
      avg -= samples[pos];
   }
   else
      ++filled;

   samples[pos] = temp;
   avg += temp;
   pos = (pos + 1)%N_SAMPLES;
}
