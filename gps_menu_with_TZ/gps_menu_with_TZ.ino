/*
 * Project: GPS Kit Display using Ublox neo-6m and ESP8266
 * Author: vino96
 * date: 09/04/2020
 */
 
 
 
 #include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define menu 8
#define enter 9
int key = 0;
double Home_LAT = 0;
double Home_LNG = 0;


//sat20x20px logo
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
static const unsigned char u8g_logo_sat[] U8X8_PROGMEM = {
  0x00, 0x01, 0x00, 0x80, 0x07, 0x00, 0xc0, 0x06, 0x00, 0x60, 0x30, 0x00,
  0x60, 0x78, 0x00, 0xc0, 0xfc, 0x00, 0x00, 0xfe, 0x01, 0x00, 0xff, 0x01,
  0x80, 0xff, 0x00, 0xc0, 0x7f, 0x06, 0xc0, 0x3f, 0x06, 0x80, 0x1f, 0x0c,
  0x80, 0x4f, 0x06, 0x19, 0xc6, 0x03, 0x1b, 0x80, 0x01, 0x73, 0x00, 0x00,
  0x66, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x70, 0x00, 0x00
};
//wave10px logo
static const unsigned char u8g2_logo_wave[] U8X8_PROGMEM ={
  0xE0, 0x03, 0x18, 0x00, 0xC4, 0x01, 0x32, 0x00, 0x8A, 0x01, 0x69, 0x00, 
  0x25, 0x00, 0x95, 0x01, 0x95, 0x01, 0x01, 0x00, };
//sat40x35px logo



// The serial connection to the GPS device
#include <SoftwareSerial.h>
static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);
//Time Library
#include <TimeLib.h>
const float UTC_offset = 5.5;
time_t prevDisplay = 0;
//GPS Library
#include <TinyGPS++.h>
TinyGPSPlus gps;

//Program variables
double Lat;
double Long;
double Alt;
//int day, month, year;
//int hour, minute, second;

int num_sat, gps_speed;
String heading;


//SETUP
void setup() {
  pinMode(menu, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP);

  ss.begin(GPSBaud);
   u8g2.begin();

//PrintingLoadingPage
    u8g2.firstPage();
  do {
    print_page1();
  } while ( u8g2.nextPage() );
  delay(5000);
  
}//END SETUP


//LOOP
void loop() {

  Get_GPS(); //Get GPS data
  if (timeStatus()!= timeNotSet) {
    if (now() != prevDisplay) {     
      prevDisplay = now();
    }
  }

  if (digitalRead(menu) == LOW)
     key = (key+1);
             //else if (digitalRead(menu) == LOW)
             //key = (key-1);
  if (key<0 or key>3)
     key = 0;


   switch (key)

{ 
  case 0:
      u8g2.firstPage();
  do {
     print_Clock();
  } while ( u8g2.nextPage() );
  delay(10);
  break;
  case 1:
   u8g2.firstPage();
  do {
   print_speed();
  } while ( u8g2.nextPage() );
  delay(10);
  break;
  case 2:
  u8g2.firstPage();
  do {
    print_location();
  } while ( u8g2.nextPage() );
  delay(10);

  break;
 case 3:
     u8g2.firstPage();
  do {
    print_Trip();
    if (digitalRead(enter) == LOW)
    { Home_LAT = gps.location.lat();
    Home_LNG = gps.location.lng(); }
    else 
    { u8g2.setFont(u8g2_font_courR08_tr);
      u8g2.setCursor(0, 64);
      u8g2.print("Press Enter to reset");
    }   
   
  } while ( u8g2.nextPage() );
  delay(10);

  break;

 }

   
} //end of loop


void print_page1() {
  u8g2.drawXBMP(0, 0, 20, 20, u8g_logo_sat);
  u8g2.setFont( u8g2_font_crox1cb_tf);
  //u8g2.setFont(u8g2_font_helvB12_tf);
  //u8g2.setFont(u8g2_font_timB12_tf);
  u8g2.setCursor(45, 20);
  u8g2.print("GPS KIT");
  //u8g2.setFont(u8g2_font_7x13B_tf);
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(55, 35);
  u8g2.print("by Dream4Tech");
  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(0, 60);
  u8g2.print("Loading");
  u8g2.setFont(u8g2_font_glasstown_nbp_tf);
  u8g2.setCursor(40, 60);
  u8g2.print(" . . . . . ");  
   
}  


void print_Clock()
  {
    u8g2.setFont(u8g2_font_courB08_tn);
    u8g2.setCursor(105, 64);
    u8g2.print( num_sat, 5);
    u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);
    u8g2.setFont(u8g2_font_crox1cb_tf);
    u8g2.setCursor(20, 10);
    u8g2.print("GPS CLOCK");
    u8g2.drawLine(0,12,128,12);
    u8g2.setFont(u8g2_font_t0_22b_tn);
    u8g2.setCursor(20, 42);
    printTime(gps.time);
    // u8g.print(gps.date);
    //Get_Date();
    u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
    u8g2.setCursor(0, 64);
    printDate(gps.date);
  
  }
  
void print_speed() {

  
 u8g2.setFont(u8g2_font_crox1cb_tf);
    u8g2.setCursor(16, 10);
    u8g2.print("Speedometer");
    u8g2.drawLine(0,15,128,15);
    
  u8g2.setFont(u8g2_font_t0_22b_tn);
  u8g2.setCursor(5, 42);
  u8g2.print(gps_speed , DEC);
  u8g2.setFont(u8g2_font_glasstown_nbp_tf);
  u8g2.setCursor(62, 42);
  u8g2.print("km/h");


  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.setCursor(105, 64);
  u8g2.print( num_sat, 5);
  u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);

  u8g2.setFont(u8g2_font_glasstown_nbp_tf);
  u8g2.setCursor(0,64);
  u8g2.print("Direction:");
  u8g2.setCursor(45,64);
  u8g2.print( heading);
}

void print_location()
{
  u8g2.setFont(u8g2_font_crox1cb_tf);
  u8g2.setCursor(10, 10);
  u8g2.print("GPS Location");
  u8g2.drawLine(0,12,128,12);

  u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
  u8g2.setCursor(5, 28);
  u8g2.print("Long: ");
  u8g2.setCursor(40, 28);
  u8g2.print( Long, 6);

  u8g2.setCursor(5, 43);
  u8g2.print("Lat: ");
  u8g2.setCursor(40, 43);
  u8g2.print( Lat, 6);

  u8g2.setCursor(0, 64);
  u8g2.print("Alt: ");
  u8g2.setCursor(20, 64);
  u8g2.print( Alt, 3);

  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.setCursor(105, 64);
  u8g2.print( num_sat, 5);
  u8g2.drawXBMP(118, 54, 10, 10, u8g2_logo_wave);
  
  }

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}


void Get_GPS()
{
  num_sat = gps.satellites.value();

  if (gps.location.isValid() == 1) {

    Lat = gps.location.lat();
    Long = gps.location.lng();
    Alt = gps.altitude.meters();
    
    gps_speed = gps.speed.kmph();

    heading = gps.cardinal(gps.course.value());
  } 
      
      int Year = gps.date.year();
      byte Month = gps.date.month();
      byte Day = gps.date.day();
      byte Hour = gps.time.hour();
      byte Minute = gps.time.minute();
      byte Second = gps.time.second();

        // Set Time from GPS data string
        setTime(Hour, Minute, Second, Day, Month, Year);
        // Calc current Time Zone time by offset value
        adjustTime(UTC_offset * SECS_PER_HOUR);           
                   
  smartDelay(1000);


  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    // Serial.println(F("No GPS detected: check wiring."));

  }
}
static void printDate(TinyGPSDate &d)
{
  if (!d.isValid())
  {
    u8g2.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", month(), day(), year());
    u8g2.print(sz);
  }
}
static void printTime(TinyGPSTime &t)
{  
  if (!t.isValid())
  {
    u8g2.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", hour(), minute(), second());
    //setTime(hour, minute, second);
    u8g2.print(sz);
  }

 // printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

void print_Trip()
{
unsigned long distanceKm =
  (unsigned long)TinyGPSPlus::distanceBetween(
    gps.location.lat(),
    gps.location.lng(),
    Home_LAT,
    Home_LNG ) / 1000.0;
      u8g2.setFont(u8g2_font_nine_by_five_nbp_tf);
      u8g2.setCursor(0, 20);
      u8g2.print("Trip: ");
      u8g2.setCursor(50, 20);
      u8g2.print(distanceKm);
      u8g2.setCursor(90, 20);
      u8g2.print("Km");
      
double courseTo =
  TinyGPSPlus::courseTo(
    gps.location.lat(),
    gps.location.lng(),
    Home_LAT,
    Home_LNG );
      
      u8g2.setCursor(0, 30);
      u8g2.print("Course: ");
      u8g2.setCursor(60, 30);
      u8g2.print(courseTo);
      u8g2.setCursor(90, 30);
      u8g2.print("Km");

String cardinalTo = TinyGPSPlus::cardinal(courseTo);
      u8g2.setCursor(0, 40);
      u8g2.print("Cardinal: ");
      u8g2.setCursor(60, 40);
      u8g2.print(cardinalTo);

}
