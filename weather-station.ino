// Display of DHT22 and BMP180 on 16x2 LCD 
// by superchunks.co.uk
// Code modified from Adafruit, reddit and Sparkfun - links below
// September 2015
// http://learn.adafruit.com/dht
// https://www.reddit.com/r/arduino/comments/18l1ix/20_minute_project_digital_temp_humidity_wlcd/
// https://www.sparkfun.com/products/11824


// Add libraries

#include <LiquidCrystal.h>
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>


// DHT22 Setup
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTPIN 2     // which pins we're connected to on arduino

// Uncomment if your using one of these other DHT sensors
// #define DHTTYPE DHT11   // DHT 11 
// #define DHTTYPE DHT21   // DHT 21 (AM2301)
// DHT Connections
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

//LCD Setup
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);  // which pins we're connected to on arduino

// BMP180 Setup
// - (GND) to GND
// + (VDD) to 3.3V
// (WARNING: do not connect + to 5V or the sensor will be damaged!)
// You will also need to connect the I2C pins (SCL and SDA) to your
// Arduino. The pins are different on different Arduinos:
// Any Arduino pins labeled:  SDA  SCL
// Uno, Redboard, Pro:        A4   A5
// Mega2560, Due:             20   21
// Leonardo:                   2    3

SFE_BMP180 pressure; 
//add a BMP180 object called "pressure"
#define ALTITUDE 42.0 // Altitude edited to 42.0m as found online - edit this value to your actual altitude

DHT dht(DHTPIN, DHTTYPE);

// start

void setup() 
{
  //Serial.begin(9600);  
  dht.begin();
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("starting");

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 its not working\n\n");
    while(1); // Pause forever.
  }
}
void loop()
{
  char status;
  double T,P,p0,a;
  
  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  
  delay(500); 
  // half second delay added to ensure first lineserial output is completed before it moves on
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");
 

// =============================================================
 status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      
      Serial.print("BMP180 temperature: ");
      Serial.print(T,2);
      Serial.print(" *C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" F");

      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          //Serial.print(P*0.0295333727,2); uncomment to show Inches of mercury
          //Serial.println(" inHg");        uncomment to show Inches of mercury

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // changed to 42m above sea level, we're at 1655 meters (Boulder, CO)
          Serial.print("\n");
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb");
          //Serial.print(p0*0.0295333727,2);  uncomment to show Inches of mercury
          //Serial.println(" inHg");          uncomment to show Inches of mercury

              }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
          }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");


// ================================================================
      
    //  void loop() {
  // Reading temperature or humidity takes about 250 milliseconds
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  //t = t* 9/5 + 32; (uncomment to read in F)

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    lcd.setCursor(0, 0);
    lcd.print(F("looking..."));
    
  } else {
   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print (p0,2);
    lcd.print("  ");
    lcd.print(T,2);
    lcd.print("\337C");
    lcd.setCursor(0, 1);
    lcd.print("H:");
    lcd.print(h);
    lcd.print("%");
    lcd.print(" ");
    lcd.print(t);
    lcd.print("\337C");
    Serial.print("\n");
    Serial.print("DHT22 humidity: "); 
    Serial.print(h);
    Serial.print("%");
    Serial.print("\n");
    Serial.print(" DHT22 temperature: "); 
    Serial.print(t);
    Serial.println("*C");
    Serial.println();
  

      // Wait 5 seconds between measurements.
  delay(5000); 
  }
}
