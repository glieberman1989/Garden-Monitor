#include <Arduino.h>

#include <Wire.h>
#include "RTClib.h"

#define manualPin 3
#define pumpPower 4

//RTC_DS3231 rtc;
RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int soilVal_1 = 0; //value for storing moisture value
const int soilPin_1 = A0;//Declare a variable for the soil moisture sensor
const int soilPower = 8;//relay for soil moisture sensor power
const int threshold = 250;//TBD - threshold value for dry vs. wet soil

//Manual Watering Interrupt Control
void manualOverride() {
  if (digitalRead(manualPin) == LOW) {
    digitalWrite(pumpPower, HIGH);
    Serial.println("Manual Overide -- Pump On");
  }
}

void setup() {
  Serial.begin(9600);
  delay(3000); // wait for console opening

  //RTC setup
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  //if (rtc.lostPower()) {
    //Serial.println("RTC lost power, lets set the time!");
    //following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

//Soil Sensor Power
  pinMode(soilPower, OUTPUT);//Set D8 as an OUTPUT
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor
//Pump Power
  pinMode(pumpPower, OUTPUT);//Set D4 as an OUTPUT
  digitalWrite(pumpPower, LOW);//Set to LOW for pump not running
//Watering Manual Overide
  pinMode(manualPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(manualPin), manualOverride, LOW);
}
//Soil Moisture Measurment Function
int readSoil() {
    digitalWrite(soilPower, HIGH);//turn D8 "On"
    delay(10);//wait 10 milliseconds
    soilVal_1 = analogRead(soilPin_1);//Read the SIG value form sensor
    digitalWrite(soilPower, LOW);//turn D8 "Off"
    return soilVal_1;//send current moisture value
}

void loop() {
  DateTime now = rtc.now();
  int soilValue;
  soilValue = readSoil();

  //Serial debug for moisture sensor
  /*Serial.print("Soil Moisture = ");
  Serial.println(readSoil());*/

  //Serial debug for RTC
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(400);

  //Main watering cycle
  if ((now.hour() == 11 && now.minute() >= 45) && (now.hour() == 11 && now.minute() < 46)) {//only water between 8AM-6PM
    digitalWrite(pumpPower, HIGH);
    Serial.println("pump ON");
    delay(500);
  }
  else {
    digitalWrite(pumpPower, LOW);
  }
}
