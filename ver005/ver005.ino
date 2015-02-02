#include <EEPROM.h>
#include <SD.h>
#include <Wire.h>
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
#define CLK_CLR    (0x11)

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
  EEPROM.write(CLK_CLR,200);
//  EEPROM.write(RCD_COUNT,0);
}




//****************************Macro definition end*************************

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
    if (clockTime==currentTime || abs(clockTime+24*60-currentTime)<1){
      
      return 1;
    }
  
}
return 0;
}

//***************************Note System***********************
int noteDown(){
  analogWrite(3,255); 
  delay(100);
  Time t=rtc.time();
  const String day = dayAsString(t.day);
  char buf[50];
   ads1115.begin();
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
           
  int16_t results;
  results=-ads1115.readADC_Differential_0_1();
  delay(50);
  
 
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
  delay(50);
  analogWrite(3,0); 
  return 0;
  
  }   
}

int noteSerial(){
  
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


void setup(void){
  Serial.begin(9600);
  int read_i=0;
  EE_ini_();
  char buf[5];
  rtc.writeProtect(true);
  rtc.halt(false);
  pinMode(13,OUTPUT);
  pinMode(3,OUTPUT);
  digitalWrite(13,LOW); 
 pinMode(10, OUTPUT);
 delay(100);
  if (!SD.begin(10)) {
   
    Serial.println("ini error");
  }
 
 myFile = SD.open("CLOCK.txt");
  int i=0;
  if (myFile) {
    Serial.println("test.txt:");
    while (myFile.available()) {
    	if (myFile.read()==';'){
           buf[0]=myFile.read();
           buf[1]=myFile.read();
           buf[2]=myFile.read();
           buf[3]=myFile.read();
           EEPROM.write(EEPROM.read(CLK_INI)+2*i,(buf[0]-'0')*10+buf[1]-'0');
           EEPROM.write(EEPROM.read(CLK_INI)+2*i+1,(buf[2]-'0')*10+buf[3]-'0');         
           i++;
       }
      
    }
     EEPROM.write(CLK_COUNT,i);
      
    // close the file:
    
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
int n=i;




Wire.begin();
analogWrite(3,0);
}



void loop(){
   
 
/*  if(chkClock()){
    noteDown();
  }
  else{
    Serial.println("Not clock time");
  }
  delay(100);
*/
int i=EEPROM.read(CLK_CLR);
    if(i<=0){
      noteDown();
      EEPROM.write(CLK_CLR,200);
    }
    else{
      i--;
      Serial.print("Not reach the deadline: ");
      Serial.print(i);
      Serial.println("left");
      EEPROM.write(CLK_CLR,i);
    }
    delay(200);
  
 
  

  
  
  // disable ADC
  ADCSRA = 0;  

  // clear various "reset" flags
  MCUSR = 0;     
  // allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE);
  // set interrupt mode and an interval 
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  wdt_reset();  // pat the dog
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  sleep_cpu ();  
  
  // cancel sleep as a precaution
  sleep_disable(); 
  

    

 
}




