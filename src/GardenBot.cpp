#include <Arduino.h>

#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_BME280.h>

#define manualIntPin 3
#define pumpPower 4

#define SEALEVELPRESSURE_HPA (1013.25)

RTC_PCF8523 rtc; //RTC_DS3231 rtc
Adafruit_BME280 bme; // temp, pressure I2C sensors

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int soilVal_1 = 0; //variable for storing moisture value
const int soilPin = A0;//pin for the soil moisture sensor
const int soilPower = 8;//relay for soil moisture sensor power
const int soilThreshold = 250;//TBD - threshold value for dry vs. wet soil

//Manual Watering Interrupt Control
void manualOverride() {
  if (digitalRead(manualIntPin) == LOW) {
    digitalWrite(pumpPower, HIGH);
    Serial.println("Manual Override -- Pump On");
  }
}

void setup() {
  Serial.begin(9600);
  delay(3000); // wait for console opening

  //BME280 setup
  bool statusBME;
  statusBME = bme.begin();
  if (! statusBME) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  //RTC setup
  bool statusRTC;
  statusRTC = rtc.begin();
  if (! statusRTC) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//sets RTC time to compile/upload time

  //Soil Sensor Power
  pinMode(soilPower, OUTPUT);//Set D8 as an OUTPUT
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensors
  //Pump Power
  pinMode(pumpPower, OUTPUT);//Set D4 as an OUTPUT
  digitalWrite(pumpPower, LOW);//Set to LOW for pump not running
  //Watering Manual Overide
  pinMode(manualIntPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(manualIntPin), manualOverride, LOW);
}

//Soil Moisture Measurment Function
int readSoil() {
  digitalWrite(soilPower, HIGH);//turn D8 "On"
  delay(10);//wait 10 milliseconds
  soilVal_1 = analogRead(soilPin);//Read the SIG value form sensor
  digitalWrite(soilPower, LOW);//turn D8 "Off"
  return soilVal_1;//send current moisture value
}

//BME280 Measurment Function
int readBME280() {
  return soilVal_1;//send current moisture value
}

//Serial Print for debugging
void debugPrint() {
  //Debug for BME280
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  Serial.println();

  //Debug for moisture sensor
  Serial.print("Soil Moisture = ");
  Serial.println(readSoil());
  Serial.println();

  delay(500);
}

void loop() {
  DateTime now = rtc.now();
  int soilValue;
  soilValue = readSoil();

  //Serial debug for sensors
  debugPrint();

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

  //Main watering cycle
  if ((now.hour() == 11 && now.minute() >= 45) && (now.hour() == 11 && now.minute() < 46)) {//water at 11:45
    digitalWrite(pumpPower, HIGH);
    Serial.println("pump ON");
    delay(100);
  }
  else {
    digitalWrite(pumpPower, LOW);
  }
}
