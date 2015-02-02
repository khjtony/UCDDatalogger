#include <EEPROM.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <stdio.h>
#include <DS1302.h>
#include <my8COM.h>
Adafruit_ADS1115 ads1115;

//***************************Macro definition**********************
#define SN_ADD1    (0x00)
#define SN1        (0x00)
#define SN_ADD2    (0x00)
#define SN2        (0x0f)
#define CLK_MODE_ADD (0x02)
#define CLK_MODE  (0x01)
#define CLK_1_ADD (0x03)
#define CLK_1     (0x00)
#define CLK_2_ADD (0x04)
#define CLK_2     (0x00)
#define CLK_3_ADD (0x05)
#define CLK_3     (0x00)
#define CLK_4_ADD (0x06)
#define CLK_4     (0x00)
#define CLK_5_ADD (0x07)
#define CLK_5     (0x00)

#define COUNT_LOWER_ADD (0x10)
#define COUNT_LOWER     (0x00)
#define COUNT_UPPER_ADD (0x11)
#define COUNT_UPPER     (0x00)
#define device 0x50  // Address with three address pins grounded. 







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
  EEPROM.write(CLK_1_ADD,CLK_1);
  EEPROM.write(CLK_2_ADD,CLK_2);
  EEPROM.write(CLK_3_ADD,CLK_3);
  EEPROM.write(CLK_4_ADD,CLK_4);
  EEPROM.write(CLK_5_ADD,CLK_5);
  EEPROM.write(COUNT_LOWER_ADD,COUNT_LOWER);
  EEPROM.write(COUNT_UPPER_ADD,COUNT_UPPER);
}
int sn;
byte clk_mode;
byte clk_1;
int clk_2;
int clk_3;
int clk_4;
int clk_5;
int count_lower;
int count_upper;
int count;
my8COM my_COM;
int dataStoreAdd=0;
int clkHour=0;  //next hour
int clkMin=0;  //next Min
int clkH;     //period of Hour
int clkM;     //period of min
int noteCount=50; //dataIndex
int iniCount=50;
//****************************Macro definition end*************************
//****************************clock system******************
int nextClock(){
  int nowH=printTime('H');
  int nowM=printTime('m');
  while (clkHour<=nowH && clkMin<=nowH){
    clkHour+=clkH; 
    clkMin+=clkM;
    if (clkMin>=60){
      clkHour+=1;
      clkMin-=60;
    }
    if (clkHour>=24){
      clkHour=0;
      clkMin=0;
    }
  }
  return 0;
}
   
  
int chkClock(){
    int nowH=printTime('H');
  int nowM=printTime('m');
  if (clkHour==nowH && clkMin==nowM){
    noteDown();
    nextClock();
  }
  return 0;
}

void setClock(byte newH,byte newM){
  EEPROM.write(CLK_1_ADD,newH);
  EEPROM.write(CLK_2_ADD,newM);
  delay(10);
}
//***************************Note System***********************
void noteDown(){
  int16_t results;
  results=-ads1115.readADC_Differential_0_1();
  EEPROM.write(noteCount*5,(byte)printTime('M'));
  EEPROM.write(noteCount*5+1,(byte)printTime('D'));
  EEPROM.write(noteCount*5+2,(byte)printTime('H'));
  EEPROM.write(noteCount*5+3,highByte(results));
  EEPROM.write(noteCount*5+4,lowByte(results));
  noteCount+=1;
  return;
  
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

int operation(){
  int i=0;
  int16_t results=0;
  switch(my_COM.readBuf()){
    case 0:     //No input
      break;
    case 1:     //Monitor
      while (true){
        my_COM.setMBuf(printTime('H'),printTime('m'),ads1115.readADC_Differential_0_1());
        my_COM.sendBuf();
        delay(1000);
      }
      
      break;
      
    case 21:    //send SN
      
      my_COM.setRBuf('S',EEPROM.read(SN_ADD1),EEPROM.read(SN_ADD2));
      my_COM.sendBuf();
      delay(100);
      break;
    case 22:    //send uptime
      
    case 23:    //send clock set
      my_COM.setRBuf('S',EEPROM.read(CLK_1_ADD),EEPROM.read(CLK_2_ADD));
      my_COM.sendBuf();
      delay(100);
      break;
    case 3:     //send all data record
      
      for(i=iniCount;i<noteCount;i++){
        results=256*EEPROM.read(i*5+3)+EEPROM.read(i*4);
        my_COM.setDBuf((int)EEPROM.read(i*5),(int)EEPROM.read(i*5+1),(int)EEPROM.read(i*5+2),results);
        my_COM.sendBuf();
        delay(100);
      }
      break;
      
  /*  case 41:    //set SN
      break;
    case 42:    //set Time
      break;
    case 43:    //set peroid
      break;
    case 44:    //erase all data
      break;
      
      */

      
  
  
  }
 // Serial.println("exit serial");
  Serial.flush();
  return 0;
  
  
}



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
//  EE_ini_();
  sn=EEPROM.read(SN_ADD1)*256+EEPROM.read(SN_ADD2);
//  if (!sn) {
 //   EE_ini_();
//  }
  clk_mode=EEPROM.read(CLK_MODE_ADD);
  clk_1=EEPROM.read(CLK_1_ADD);
  clkH=(int)clk_1;
  clk_2=EEPROM.read(CLK_2_ADD);
  clkM=(int)clk_2;
  clk_3=EEPROM.read(CLK_3_ADD);
  clk_4=EEPROM.read(CLK_4_ADD);
  clk_5=EEPROM.read(CLK_5_ADD);
  
  clk_1=EEPROM.read(CLK_1_ADD);
  count_upper=EEPROM.read(COUNT_UPPER_ADD);
  count_lower=EEPROM.read(COUNT_LOWER_ADD);

  rtc.writeProtect(true);
  rtc.halt(false);
  


  //*****ads1115*****
  ads1115.begin();
}



void loop(){
  int i=0x01;
  for (i=0x01;i<10;i++){
    int     shift_i=i*8;
    deviceWrite(printTime('Y')-2000,shift_i);
    //Serial.print("Year is ");
   // Serial.print(printTime('Y')-2000);
    Serial.print(deviceRead(shift_i),DEC);
    Serial.print(",");
    deviceWrite(printTime('M'),shift_i+1);
    Serial.print(deviceRead(shift_i+1),DEC);
    Serial.print(",");
    deviceWrite(printTime('D'),shift_i+2);
    Serial.print(deviceRead(shift_i+2),DEC);
    Serial.print(",");
    deviceWrite(printTime('H'),shift_i+3);
    Serial.print(deviceRead(shift_i+3),DEC);
    Serial.print(",");
    deviceWrite(printTime('m'),shift_i+4);
    Serial.print(deviceRead(shift_i+4),DEC);
    Serial.print(",");
    deviceWrite(printTime('s'),shift_i+5);
    Serial.print(deviceRead(shift_i+5),DEC);
    Serial.print(",");
    deviceWrite(shift_i+6,shift_i+6);
    Serial.print(deviceRead(shift_i+6),DEC);
    Serial.print(",");
   deviceWrite(shift_i+7,shift_i+7);
  Serial.print(deviceRead(shift_i+7),DEC);
    
    Serial.print("\n");
    delay(2000);
  }
  Serial.println("wrote done.");
  delay(1000);
 
}




