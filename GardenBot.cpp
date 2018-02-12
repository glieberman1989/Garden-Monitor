#include <Arduino.h>
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int soilVal_1 = 0; //value for storing moisture value
int soilPin_1 = A0;//Declare a variable for the soil moisture sensor
int soilPower = 8;//Variable for Soil moisture Power
int soilThres = 80;//TBD - threshold value for dry vs. wet soil

int pumpPower = 4;//relay for turning pump off and on

const unsigned long interval = 3UL*60UL*60UL*1000UL;//run watering loop every 3 hours

void setup() {
//RTC setup
  Serial.begin(9600);
  delay(3000); // wait for console opening
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

void loop() {
  DateTime now = rtc.now();

  while (now.hour() >= 8 && now.hour() <= 18 && now.minute() == 00 && now.second() == 00)//only water between 8AM-6PM
  {
    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds
    soilVal_1 = analogRead(soilPin_1);//Read the value form sensor
    digitalWrite(soilPower, LOW);//turn D7 "Off"
    if (soilVal_1 < soilThres) {//if soil wetness is below the threshold value, start watering
      digitalWrite(pumpPower, HIGH);
      delay(60000);//water for 1 minute
      digitalWrite(pumpPower, LOW);
    }
    delay(interval);
  }
}
