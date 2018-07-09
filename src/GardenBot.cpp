#include <Arduino.h>

#include <Wire.h>
#include "RTClib.h"
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib

//RTC_DS3231 rtc;
RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int soilVal_1 = 0; //value for storing moisture value
const int soilPin_1 = A0;//Declare a variable for the soil moisture sensor
const int soilPower = 8;//relay for soil moisture sensor power
const int threshold = 250;//TBD - threshold value for dry vs. wet soil
const int manualPin = 3;
const int pumpPower = 4;//relay for turning pump off and on
unsigned long pumpStart;
unsigned long pumpEnd;
volatile int manualState = 0;

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
/*//Watering Manual Overide
  pinMode(manualPin, INPUT);
  attachInterrupt(0, manualOverride, CHANGE);*/
}
//Soil Moisture Measurment Function
int readSoil() {
    digitalWrite(soilPower, HIGH);//turn D8 "On"
    delay(10);//wait 10 milliseconds
    soilVal_1 = analogRead(soilPin_1);//Read the SIG value form sensor
    digitalWrite(soilPower, LOW);//turn D8 "Off"
    return soilVal_1;//send current moisture value
}
/*//Manual Watering Interrupt Control
void manualOverride() {
  manualState = digitalRead(manualPin);
  digitalWrite(manualPin, manualState);
}
*/
void loop() {
  DateTime now = rtc.now();
  int sensorValue;
  sensorValue = readSoil();

  //Serial debug for moisture sensor
  /*Serial.print("Soil Moisture = ");
  Serial.println(readSoil());*/

  //Serial debug for RTC
  /*Serial.print(now.year(), DEC);
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
  delay(3000);*/

  //Main watering cycle
  if ((now.hour() == 11 && now.minute() >= 45) && (now.hour() == 11 && now.minute() < 46)) {//only water between 8AM-6PM
    digitalWrite(pumpPower, HIGH);
    Serial.println("pump ON");
    delay(500);
    /*//if (sensorValue <= threshold) {
    pumpStart = millis();
    pumpEnd = pumpStart;
      while ((pumpEnd - pumpStart) <=60000) {
        digitalWrite(pumpPower, HIGH);
        Serial.println("pump ON");
        pumpEnd = millis();
        delay(500);
      }
      digitalWrite(pumpPower, LOW);*/
  }
  else {
    digitalWrite(pumpPower, LOW);
  }
}
