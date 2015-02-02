#include <EEPROM.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <stdio.h>
#include <DS1302.h>
Adafruit_ADS1115 ads1115;

//***************************Macro definition**********************
#define SN_ADD    (0x00)
#define SN        (0x00)
#define CLK_MODE_ADD (0x01)
#define CLK_MODE  (0x00)
#define CLK_1_ADD (0x02)
#define CLK_1     (0x00)
#define CLK_2_ADD (0x03)
#define CLK_2     (0x00)
#define CLK_3_ADD (0x04)
#define CLK_3     (0x00)
#define CLK_4_ADD (0x05)
#define CLK_4     (0x00)
#define CLK_5_ADD (0x06)
#define CLK_5     (0x00)










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

//****************************Macro definition end*************************


void setup(void){
  Serial.begin(9600);
  


}




