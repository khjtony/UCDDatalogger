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
#define CLK_MODE_ADD (0x02)
#define CLK_MODE  (0x01)

#define CLK_FLAG   (5)
#define CLK_COUNT  (6)
#define CLK_count  (5)
#define RCD_COUNT  (0x7)


#define CLK1_H    (10)
#define CLK1_M    (11)
#define CLK2_H    (12)
#define CLK2_M    (13)
#define CLK3_H    (14)
#define CLK3_M    (15)
#define CLK4_H    (16)
#define CLK4_M    (17)
#define CLK5_H    (18)
#define CLK5_M    (19)



#define device 0x50  // Address with three address pins grounded. 


int rcd_count=0;




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
  EEPROM.write(CLK_MODE_ADD,CLK_MODE);
  EEPROM.write(0,CLK_FLAG);
  EEPROM.write(CLK_count,CLK_COUNT);
//  EEPROM.write(RCD_COUNT,0);
}

void setClock(int Hour,int Min,int add){
  EEPROM.write(Hour,add*2+8);
  delay(10);
  EEPROM.write(Min,add*2+9);
  delay(10);
}


//****************************Macro definition end*************************
//****************************clock system******************
int chkClock(){
  int roof=EEPROM.read(CLK_COUNT);
  int i=0;
  
  for (i=0;i<roof;i++){
    if(abs(printTime('H')*3600+printTime('m')*60+printTime('s')-EEPROM.read(i*2+8)*3600-EEPROM.read(i*2+11))<9){
      return 1;
    }
  return 0;
}
}

//***************************Note System***********************
int noteDown(){
  int16_t results;
  results=-ads1115.readADC_Differential_0_1();
  int addr=EEPROM.read(RCD_COUNT)*8;
// int addr=rcd_count*8;
  deviceWrite(printTime('Y')-2000,addr);
  deviceWrite(printTime('M'),addr+1);
  deviceWrite(printTime('D'),addr+2);
  deviceWrite(printTime('H'),addr+3);
  deviceWrite(printTime('m'),addr+4);
  deviceWrite(printTime('s'),addr+5);
  deviceWrite(highByte(results),addr+6);
  deviceWrite(lowByte(results),addr+7);
  EEPROM.write(RCD_COUNT,EEPROM.read(RCD_COUNT)+1);
  return 1;
   
}

byte noteGet(int currentNote,char option){
  byte Month=EEPROM.read(currentNote*5);
  byte Day=EEPROM.read(currentNote*5+1);
  byte Hour=EEPROM.read(currentNote*5+2);
  byte High=EEPROM.read(currentNote*5+3);
  byte Low=EEPROM.read(currentNote*5+4);
  
  switch(option){
    case 'M':
      return Month;
    case 'D':
      return Day;
    case 'H':
      return Hour;
    case '1':
      return High;
    case '2':
      return Low;
    default:
      return Month;
  }
  return 0;
}

void notePrint(int option){
  int i=0;
  int16_t results;
  if (option==0){
  for(i=0;i<EEPROM.read(RCD_COUNT);i++){
    Serial.print(deviceRead(i*8),DEC);
    Serial.print(',');
    Serial.print(deviceRead(i*8+1),DEC);
    Serial.print(',');
    Serial.print(deviceRead(i*8+2),DEC);
    Serial.print(',');
    Serial.print(deviceRead(i*8+3),DEC);
    Serial.print(',');
    Serial.print(deviceRead(i*8+4),DEC);
    Serial.print(',');
    Serial.print(deviceRead(i*8+5),DEC);
    Serial.print(',');
    results=deviceRead(i*8+6)|deviceRead(i*8+7);
    Serial.print(results*0.188);
    Serial.print('\n');
  }
  }
  else{
    for(i=0;i<EEPROM.read(RCD_COUNT);i++){
    Serial.print('A');
    Serial.print('B');
    Serial.print('C');
    Serial.print('D');
    Serial.print('E');
    Serial.print('F');
    Serial.print('G');
    results=deviceRead(i*8+6)|deviceRead(i*8+7);
    Serial.print('H');
    Serial.print('Z');
  }
    
    
    
  }
    
}
  
//***************************Watch dog system**********************
// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect
//***************************watch dog system end*****************



//****************************Communication system************************
//****************************EEPROM*******************
void deviceWrite(byte data, byte wordaddress) {
  Wire.beginTransmission(device);
  Wire.write(wordaddress);
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}

byte deviceRead(byte wordaddress) {
  Wire.beginTransmission(device);
  Wire.write(wordaddress);
  Wire.endTransmission();

  Wire.requestFrom(device, 1);
  if(Wire.available())
    return Wire.read();
  else
    return 0xFF;
}

//***************************EEPROM END******************

void setup(void){
  Serial.begin(9600);
  Wire.begin();
  EE_ini_();
  int sn=EEPROM.read(SN_ADD1)*256+EEPROM.read(SN_ADD2);

  rtc.writeProtect(true);
  rtc.halt(false);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  //*****ads1115*****
  ads1115.begin();
  
  setClock(9,30,1); 
   setClock(9,31,1); 
    setClock(9,35,1); 
     setClock(9,36,1); 
      setClock(9,42,1); 
      
  
}



void loop(){
  delay(100);
 // ads1115.begin();
  noteDown();
 notePrint(1);

  if (EEPROM.read(RCD_COUNT)==31){
    EEPROM.write(RCD_COUNT,0);
  }
  delay(100);
  
 // delay(1000);
  
  
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




