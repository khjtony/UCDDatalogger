#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <stdio.h>
#include "DS3231.h"
Adafruit_ADS1115 ads1115;
int choice=0;


// Example sketch for interfacing with the DS1302 timekeeping chip.
//
// Copyright (c) 2009, Matt Sparks
// All rights reserved.
//
// http://quadpoint.org/projects/arduino-ds1302


namespace {

// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:
//
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 5;  // Chip Enable
const int kIoPin   = 6;  // Input/Output
const int kSclkPin = 7;  // Serial Clock

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}

void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);
}

}  // namespace


void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
  pinMode(3,OUTPUT);
  analogWrite(3,255);
//  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
//  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
//  ads1115.begin();
  
  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  Time t(2014, 2, 22, 13, 58, 50, Time::kSaturday);

  // Set the time and date on the chip.
  rtc.time(t);
  
  
}
 
void loop(void)
{
   if (Serial.available() > 0) {
                // read the incoming byte:
                choice = (int)Serial.read()-48;
              
   }
 if (choice==1){
   analogWrite(3,255);
   ads1115.begin();
  int16_t results;
  results = ads1115.readADC_Differential_0_1();
  Serial.print("Differential: "); Serial.print(results); Serial.print("("); Serial.print(results * 0.188); Serial.println("mV)");
  analogWrite(3,0);
  delay(1000);
  
 }
 else{
   printTime();
  delay(1000);
 }
 
}
