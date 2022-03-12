#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <ezButton.h>

#include "DCF77.h" //Alle DCF77 Funktionen laufen ueber diese Bibliothek.
#include "TimeLib.h" //Time.h hat bei mir nie funktioniert, habe TimeLib.h eingebunden, dann gehts.

#define  PIXELS              60             // total number of Neo-Pixels
#define  RINGS                6             // led rings output       

#define  DCF77_PIN            2             // DFC77 pin              
#define  ONE_SECOND_INTERUPT  0             // 1Hz on inturrupt 0    

#define  button_clockface     4

DCF77 DCF = DCF77(DCF77_PIN, ONE_SECOND_INTERUPT, false);

Adafruit_NeoPixel ring = Adafruit_NeoPixel(PIXELS, RINGS, NEO_GRB + NEO_KHZ800);
ezButton button(button_clockface);

bool show_clockface = false;

int lastButtonState;    // the previous state of button
int currentButtonState; // the current state of button

boolean hourOffset  = 0; 

time_t prevDisplay = 0;

void setup()
{
    Serial.begin(115200);
    ring.begin(); 
    ring.setBrightness(20);
    ring.show(); 
    DCF.Start();
    setTime(4,59,34, 1, 1, 1970);
    setSyncInterval(30);
    setSyncProvider(ReadTimeDate);

    while(timeStatus()== timeNotSet) { 
        // wait until the time is set by the sync provider     
        Serial.print(".");
        delay(2000);
    }
}

void loop()
{
    if ( now() != prevDisplay){
        prevDisplay = now();
    }

    button.loop();


    if (button.isPressed()) {
      Serial.println("button pressed");
      show_clockface = !show_clockface;
    }

    if (show_clockface) {
      drawClockFace();
      displayTime(); 
    }
    else{
      ring.clear();
      displayTime();  
    }

    delay(300);
}

unsigned long ReadTimeDate(){
    time_t DCFtime = DCF.getTime();
	
    if(DCFtime!=0){
          
    }
    return DCFtime;
}

void displayTime() {
    printouttime();
    
    if (minute() > 30) { hourOffset = 1; }         // calc hour hand offset
    else             { hourOffset = 0; }
    // set colors
    ring.setPixelColor  ( second() + offset(second()), ring.Color(   56, 31, 81 ) ); //60
    ring.setPixelColor  ( minute() + offset(minute()), ring.Color( 240,  40, 0 ) ); //60
    ring.setPixelColor  ( (hourFormat12() * 5 + ( round(minute() / 15) )) + houroffset(hourFormat12()), ring.Color( 128, 128, 0 ) ); //60
    
    ring.show();                                // display new time
}

int houroffset(int value){
  if (value <= 5 )
      return 30;
  else
      return -30;
}

int offset(int value){
   if (value < 30)
     return 30;
   else
     return -30;
}

void printouttime( ) {
    Serial.println("");
    Serial.print("Time: ");
    Serial.print(hour());
    Serial.print(":");
    printDigits(minute());
    Serial.print(":");
    printDigits(second());
    Serial.print(" Date: ");
    Serial.print(day());
    Serial.print(".");
    Serial.print(month());
    Serial.print(".");
    Serial.println(year());
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void drawClockFace() {
  byte index;
  
  // draw 12 ring faint red, 24 ring faint magenta, 60 ring faint blue with white tick marks every 5
  for ( index =  0; index < 12; index++ ) { ring.setPixelColor(  index +24 + 60, ring.Color(  8,  0,  0 ) ); } 
  for ( index =  0; index < 24; index++ ) { ring.setPixelColor(  index + 60 , ring.Color(  8,  0,  8 ) ); }
  for ( index =  0; index < 60; index++ ) { 
    if( index % 5 == 0)                   { ring.setPixelColor(  index , ring.Color( 32, 40, 40 ) ); } 
    else                                  { ring.setPixelColor( index , ring.Color(  0,  0, 8 ) ); } 
  }
}