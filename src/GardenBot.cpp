#include <Arduino.h>
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

int soilVal_1 = 0; //value for storing moisture value
int threshold = 250; //*Change this value based on calibration values

const int soilPin_1 = A0;//Declare a variable for the soil moisture sensor
const int soilPower = 8;//Variable for Soil moisture Power
const int soilThres = 80;//TBD - threshold value for dry vs. wet soil
const int pumpPower = 4;//relay for turning pump off and on

const unsigned long interval = 3UL*60UL*60UL*1000UL;//run watering loop every 3 hours

void setup() {
  Serial.begin(9600);
  delay(3000); // wait for console opening

  //RTC setup
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
//Soil sensor power
  pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor
//Pump Power
  pinMode(pumpPower, OUTPUT);//Set D4 as an OUTPUT
  digitalWrite(pumpPower, LOW);//Set to LOW for pump not running
}

int readSoil() {
    digitalWrite(soilPower, HIGH);//turn D8 "On"
    delay(10);//wait 10 milliseconds
    soilVal_1 = analogRead(soilPin_1);//Read the SIG value form sensor
    digitalWrite(soilPower, LOW);//turn D8 "Off"
    return soilVal_1;//send current moisture value
}

void loop() {

  DateTime now = rtc.now();
  int sensorValue;
  sensorValue = readSoil();

  while (now.hour() >= 8 && now.hour() <= 18) {//only water between 8AM-6PM
    /*Serial.print("Soil Moisture = ");
    //get soil moisture value from the function below and print it
    Serial.println(readSoil());*/
    if (sensorValue <= threshold) {
      digitalWrite(pumpPower, HIGH);
      delay(60000);//water for 1 minute
      digitalWrite(pumpPower, LOW);
    }
    delay(interval);
  }
}
