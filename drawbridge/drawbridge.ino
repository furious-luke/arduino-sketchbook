#include <debug.h>
#include <Arduino.h>
#include <SPI.h>
#include <LowPower.h>
#include <WiFly.h>
#include "credentials.h"

WiFlyDevice wifly;
WiFlyServer server( wifly );

// What state is the bridge in?
int dwell_time = 3;

// After this many iterations check if we're connected still.
unsigned long check_delay = 600000; // every ten minutes
unsigned long check_begin;

// After this much time, force reconnect.
unsigned long alive_delay = 3600000; // every hour
unsigned long alive_begin;

///
///
///
void
join()
{
   LOGLN( "Joining network." );
   while( 1 )
   {
      if( wifly.join( ssid, passphrase ) )
         break;
      delay( 10000 );
   }
   LOGLN( "Joined wifi network." );
}

///
///
///
void
check_associated()
{
   // if( (millis() - check_begin) >= check_delay )
   // {
   //    LOGLN( "Checking connection." );
   //    wifly.getConnection();
   //    if( !wifly.isAssociated() )
   //    {
   //       LOGLN( "Lost connection." );
   //       force_join();
   //    }
   //    else
   //    {
   //       LOGLN( "Connection okay." );
   //    }
   //    check_begin = millis();
   // }
}

///
///
///
void
keep_alive()
{
   if( (millis() - alive_begin) >= alive_delay )
   {
      LOGLN( "Reconnecting to keep alive." );
      join();
      alive_begin = millis();
   }
}

///
///
///
void
raise_bridge( unsigned time )
{
   LOGLN( "Raising bridge." );
   digitalWrite( 2, HIGH );

   // Keep going until switch is pushed or time elapsed.
   // digitalWrite( 4, HIGH );
   // int state;
   // unsigned long begin = millis();
   // do
   // {
   //    // delay( 10 );
   //    // state = digitalRead( 5 );
   // }
   // while( /* state == 0 && */(millis() - begin) < time );
   delay( time*1000 );

   digitalWrite( 2, LOW );
   // digitalWrite( 4, LOW );
   LOGLN( "Done." );
}

///
///
///
void
lower_bridge( unsigned time )
{
   LOGLN( "Lowering bridge." );
   digitalWrite( 3, HIGH );

   // Keep going until switch is pushed or time elapsed.
   delay( time*1000 );
   digitalWrite( 3, LOW );
   LOGLN( "Done." );
}


///
///
///
void
sleep( unsigned time )
{
   LOGLN( "Sleeping." );
   wifly.sleep( time );

   // How many iterations of 8s sleeps?
   unsigned its = time/8 + 1;
   while( its-- )
      LowPower.powerDown( SLEEP_8S, ADC_OFF, BOD_OFF );

   // I think I need to reset everything.
   LOGLN( "Waking up." );
   delay( 1000 );
   wifly.begin();
   join();
}

///
///
///
unsigned
parse_time( String& data )
{
   unsigned time;
   if( sscanf( &data[0], "%d", &time ) == 1 )
   {
      LOG( "Found time: " );
      LOGVLN( time );
      return time;
   }
   LOGLN( "Using default time." );
   return dwell_time;
}

///
///
///
void
send_header_ok( WiFlyClient& client )
{
   client.println( F( "HTTP/1.1 200 OK" ) );
   client.println( F( "Content-Type: text/html" ) );
   client.println();
}

///
///
///
void
send_controls( WiFlyClient& client )
{
   LOGLN( "Sending controls page." );
   send_header_ok( client );
   client.println( F( "<html>" ) );
   client.println( F( "<title>Drawbridge</title>" ) );
   client.println( F( "<body>" ) );
   client.println( F( "<h1>Drawbridge</h1>" ) );
   client.println( F( "<form action=\"/\" method=\"post\">" ) );
   client.println( F( "<input type=\"text\" name=\"raise_time\" value=\"22\" />" ) );
   client.println( F( "<input type=\"submit\" value=\"Raise\" />" ) );
   client.println( F( "</form>" ) );
   client.println( F( "<form action=\"/\" method=\"post\">" ) );
   client.println( F( "<input type=\"text\" name=\"lower_time\" value=\"17\" />" ) );
   client.println( F( "<input type=\"submit\" value=\"Lower\" />" ) );
   client.println( F( "</form>") );
   client.println( F( "<form action=\"/\" method=\"post\">" ) );
   client.println( F( "<input type=\"text\" name=\"sleep_time\" value=\"36000\" />" ) );
   client.println( F( "<input type=\"submit\" value=\"Sleep\" />" ) );
   client.println( F( "</form>") );
   client.println( F( "</body>" ) );
   client.println( F( "</html>" ) );
   client.println();
}

///
///
///
void
send_message( WiFlyClient& client,
              const __FlashStringHelper* msg )
{
   send_header_ok( client );
   client.println( F( "<html>" ) );
   client.println( F( "<title>Drawbridge</title>" ) );
   client.println( F( "<body>" ) );
   client.println( F( "<h1>Drawbridge</h1>" ) );
   client.println( F( "<p>" ) );
   client.println( msg );
   client.println( F( "</p>" ) );
   client.println( F( "<p>" ) );
   client.println( F( "<a href=\"/\">Back</a>" ) );
   client.println( F( "</p>" ) );
   client.println( F( "</body>" ) );
   client.println( F( "</html>" ) );
   client.println();
 }

///
///
///
void
send_404( WiFlyClient& client )
{
   client.println( F( "HTTP/1.1 404 Not found" ) );
   client.println( F( "Content-Type: text/html" ) );
   client.println();
   client.println( F( "<html>" ) );
   client.println( F( "<title>Drawbridge</title>" ) );
   client.println( F( "<body>" ) );
   client.println( F( "<h1>404</h1>" ) );
   client.println( F( "</body>" ) );
   client.println( F( "</html>" ) );
   client.println();
}

///
///
///
void
setup()
{
   // Setup the serial port.
   LOG_INIT( 9600 );
   LOGLN( "Drawbridge WebServer" );

   // Prepare my input/output pins.
   pinMode( 2, OUTPUT );
   pinMode( 3, OUTPUT );
   pinMode( 4, OUTPUT );
   pinMode( 5, INPUT );

   // Initialise the wifi.
   wifly.begin();
   LOGLN( "Initialised." );

   // Connect.
   join();

   // Prepare the server.
   server.begin();

   // Setup timed events.
   check_begin = millis();
   alive_begin = millis();

   LOGLN( "Entering main loop." );
}

///
///
///
void
loop()
{
   // check_associated();
   keep_alive();

   WiFlyClient client( wifly );
   if( server.available( client ) )
   {
      int match = client.match_P( 4, F("GET / "), F( "raise_time=" ), F( "lower_time=" ), F( "sleep_time=" ) );
      String data;
      int time;
      switch( match )
      {
         case 0:
            LOGLN( "GET request." );
            send_controls( client );
            delay( 500 );
            client.close();
            delay( 500 );
            break;

         case 1:
            data = client.readString();
            time = parse_time( data );
            LOG( "Raising for " );
            LOGV( time );
            LOGLN( " s." );
            send_message( client, F( "Raising bridge." ) );
            raise_bridge( time );
            client.close();
            break;

         case 2 :
            data = client.readString();
            time = parse_time( data );
            LOG( "Lowering for " );
            LOGV( time );
            LOGLN( " s." );
            send_message( client, F( "Lowering bridge." ) );
            lower_bridge( time );
            client.close();
            break;

         case 3:
            data = client.readString();
            time = parse_time( data );
            send_message( client, F( "Sleeping." ) );
            delay( 500 );
            client.close();
            delay( 500 );
            LOG( "Sleeping for " );
            LOGV( time );
            LOGLN( " s." );
            sleep( time );
            break;

         default:
            LOGLN( "Match failed." );
            send_404( client );
            client.close();
            break;
      }
   }
}
