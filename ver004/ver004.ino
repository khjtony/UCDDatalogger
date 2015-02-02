#include <EEPROM.h>
#include <SD.h>
#include<Wire.h>
#include <Adafruit_ADS1015.h>
#include <stdio.h>
#include <DS1302.h>
Adafruit_ADS1115 ads1115;
#include <avr/sleep.h>
#include <avr/wdt.h>

//***************************Macro definition**********************
#define SN_ADD1    (0x00)
#define SN1        (0x00)
#define SN_ADD2    (0x01)
#define SN2        (0x0f)
#define CLK_COUNT  (0x05)
#define CLK_INI    (0x10)

File myFile;





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

int printTime(char option) {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
//  char buf[50];
 // snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
  //         day.c_str(),
   //        t.yr, t.mon, t.date,
    //       t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
//  Serial.println(buf);
  switch(option){
    case 'Y':
        return t.yr;
    case 'M':
        return t.mon;
    case 'D':
        return t.date;
    case 'H':
        return t.hr;
    case 'm':
        return t.min;
    case 's':
        return t.sec;
    default:
        return t.mon;
  }


}

}  // namespace

void EE_ini_(){
  EEPROM.write(SN_ADD1,SN1);
  EEPROM.write(SN_ADD2,SN2);
//  EEPROM.write(RCD_COUNT,0);
}




//****************************Macro definition end*************************
int msg_blink(int option){
  switch (option){
    case 0:
        digitalWrite(13,HIGH);
        delay(100);
        digitalWrite(13,LOW);
        delay(100);
        digitalWrite(13,HIGH);
        delay(100);
        digitalWrite(13,LOW);
        delay(100);
        digitalWrite(13,HIGH);
        delay(100);
        digitalWrite(13,LOW);
        delay(100);
        break;
    case 1:
        digitalWrite(13,HIGH);
        delay(1000);
        digitalWrite(13,LOW);
        delay(100);
        digitalWrite(13,HIGH);
        delay(1000);
        digitalWrite(13,LOW);
        delay(100);
        digitalWrite(13,HIGH);
        delay(1000);
        digitalWrite(13,LOW);
        break;
    case 2:
        digitalWrite(13,HIGH);
        delay(1000);
        digitalWrite(13,LOW);
        delay(100);
        digitalWrite(13,HIGH);
        delay(100);
        digitalWrite(13,LOW);
        delay(100);
        digitalWrite(13,HIGH);
        delay(1000);
        digitalWrite(13,LOW);
        break;
  }
  return 0;
}
//****************************clock system******************
int chkClock(){
  int  roof=EEPROM.read(CLK_COUNT);
  int i=0;
  int addr=EEPROM.read(CLK_INI);
  int currentTime=printTime('H')*60+printTime('m');
  Serial.print("Now is");
  Serial.println(currentTime);
  int clockTime=0;
  for (i=0;i<roof;i++){
    clockTime=EEPROM.read(i*2+addr)*60+EEPROM.read(i*2+addr+1);
    Serial.print("Now check");
    Serial.println(clockTime);
    if (abs(clockTime-currentTime)<1 || abs(clockTime+24*60-currentTime)<1){
      
      return 1;
    }
  
}
return 0;
}

//***************************Note System***********************
int noteDown(){
  Time t=rtc.time();
  const String day = dayAsString(t.day);
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
           
  int16_t results;
  results=-ads1115.readADC_Differential_0_1();
  
  
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile){
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(buf);
  myFile.print(",");
  myFile.println(results);
  myFile.close(); 
  Serial.println("Noted Down at");
  Serial.println(buf); 
  return 0;
  }   
}

int noteSerial(){
  if (!SD.begin(10)) {
    msg_blink(2);
    return 0;
  }
  myFile=SD.open("DATA.txt");
  while (myFile.available()) {
    	Serial.write(myFile.read());
    }
  return 0;
}


    
 
  
//***************************Watch dog system**********************
// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect
//***************************watch dog system end*****************



//****************************Communication system************************


void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);
   
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  // re-open the file for reading:
  char buf[100];
  myFile = SD.open("CLOCK.txt");
  int i=0;
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	buf[i]=myFile.read();
        i++;
    }
    
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
int n=i;
Serial.println("BEGIN");
for (i=0;i<n;i++){
  if (buf[i]==';'){
    Serial.print(buf[i+1]);
    Serial.print(buf[i+2]);
    Serial.print(':');
    Serial.print(buf[i+3]);
    Serial.println(buf[i+4]);
  }
}


}

void loop()
{
	 myFile = SD.open("test.txt", FILE_WRITE);
   Time t=rtc.time();
  const String day = dayAsString(t.day);
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println(buf);
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  delay(1000);	// nothing happen// nothing happens after setup
}




