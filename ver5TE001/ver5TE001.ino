#include <EEPROM.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX
#include <avr/sleep.h>
#include <avr/wdt.h>

RTC_DS1307 rtc;

//************************************Macro definition*****************
#define SN_ADD1    (0x00)
#define SN1        (0x00)
#define SN_ADD2    (0x01)
#define SN2        (0x0f)
#define CLK_COUNT  (0x05)
#define CLK_INI    (0x10)

File myFile;
int SD_SE=10;

//************************************void EE_ini();
void EE_ini_(){
  EEPROM.write(SN_ADD1,SN1);
  EEPROM.write(SN_ADD2,SN2);
//  EEPROM.write(RCD_COUNT,0);
}

//****************************clock system******************
int chkClock(){
  analogWrite(6,255);
  rtc.begin();
  DateTime now = rtc.now();
  analogWrite(6,0);
  int  roof=EEPROM.read(CLK_COUNT);
  int i=0;
  int addr=EEPROM.read(CLK_INI);
  int currentTime=now.hour()*60+now.minute();
  Serial.print("Now is");
  Serial.println(currentTime);
  int clockTime=0;
  for (i=0;i<roof;i++){
    clockTime=EEPROM.read(i*2+addr)*60+EEPROM.read(i*2+addr+1);
 //   Serial.print("Now check");
 //   Serial.println(clockTime);
    if (clockTime==currentTime || abs(clockTime+24*60-currentTime)<1){
      
      return 1;
    }
  
}
return 0;
}


//***********note down for 5TE**************
int noteDown(int sn){
  byte income;
  int i=0;
  float mos;
  float temp;
  byte raw[20];
  analogWrite(3,255);
  delay(60);
  while (mySerial.available()){
    income=mySerial.read();
    raw[i]=income;
    i++;
    if (income==0x0A){
      analogWrite(3,0);
      break;
    }
  }
    mos=TE_mos(raw);
    temp=TE_temp(raw);
    
    
  analogWrite(6,255);
  rtc.begin();
  DateTime now = rtc.now();
  analogWrite(6,0);    
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.day(),
           now.hour(), now.minute());

  
 
  
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile){
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(buf);
  myFile.print(",");
  myFile.print(mos);
  myFile.print(",");
  myFile.println(temp);
  myFile.close(); 
  Serial.println("Noted Down at");
  Serial.println(buf); 
  return 0;
  }   
}


void noteShow(){
   myFile=SD.open("DATA.txt");
   while(myFile.available()){
     Serial.write(myFile.read());
   }
   myFile.close(); 
   return;
}

//********************5TE
float TE_mos(byte* raw){
  float mos;
  int i=1;
  while (raw[i]!=' '){
       mos=mos*10+raw[i]-'0';
       i++;
     }
  mos=mos/50;   
  return mos;
}

float TE_temp(byte* raw){
  int i=0;
  int roof=0;
  while (raw[roof]!='z'){
    roof++;}
  float temp=0;
  roof-=2;
  while (raw[roof]!=' '){
    temp=temp+pow(10,i)*(raw[roof]-'0');
    i++;
    roof--;
  }
  temp=(temp-400)/10;
  return temp;
}




//***************************Watch dog system**********************
// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect
//***************************watch dog system end*****************

//****************************setup*******************
void setup(void){
  Serial.begin(9600);
  int read_i=0;
  EE_ini_();
  char buf[5];
  pinMode(13,OUTPUT);
  pinMode(3,OUTPUT);  //5TE POWER
  pinMode(6,OUTPUT);  //Clock POWER
  pinMode(4,INPUT);
  pinMode(10, OUTPUT);
  analogWrite(3,0);
  
 mySerial.begin(1200);
  if (!SD.begin(SD_SE)) {

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
    Serial.println("error opening CLOCK.txt");
  }
Wire.begin();

}

//*************************loop**************8
void loop(){
  //if (chkClock()){
   if(1==1){
  noteDown(0);
 // noteDown(2);
 // noteShow();
  delay(10);
  }
  else
  {
    Serial.println("Clock not reach");
    delay(30);
  }
  
 
  

  
  
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

