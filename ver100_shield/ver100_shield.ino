#include <EEPROM.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 8); // RX, TX
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Adafruit_ADS1015.h>


RTC_DS1307 rtc;
//***********************Wire setup
/*Clock
vcc--6
gnd--gnd
scl-scl
sda-sda


SD
CS--10
DI--11
DO--12
CLK--13
vcc--3

5TE
softseries
in--4
*/

//************************************Macro definition*****************
#define SN_ADD1    (0x00)
#define SN1        (0x00)
#define SN_ADD2    (0x01)
#define SN2        (0x0f)
#define CLK_COUNT  (0x05)
#define CLK_INI    (0x10)
#define CLK_VCC    3
#define PUB_VCC    3
#define TE_IN      4
#define TE_VCC     6
File myFile;
#define SD_SE      10
DateTime now;
//************************************void EE_ini();
void EE_ini_(){
  EEPROM.write(SN_ADD1,SN1);
  EEPROM.write(SN_ADD2,SN2);
//  EEPROM.write(RCD_COUNT,0);
}

//****************************clock system******************
int chkClock(){
  
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
int noteDown_5TE(){
  byte income;
  int i=0;
  
  byte raw[20];
  analogWrite(TE_VCC,255);
  delay(60);
  while (mySerial.available()){
    income=mySerial.read();
    raw[i]=income;
    i++;
    if (income==0x0A){
      analogWrite(TE_VCC,0);
      break;
    }
  }
    
    
    
   
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.day(),
           now.hour(), now.minute());
  myFile = SD.open("5TE_DATA.txt", FILE_WRITE);
  if (myFile){
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(buf);
  myFile.print(",");
  myFile.print(TE_measure(raw,1));
  myFile.print(",");
  myFile.print(TE_measure(raw,2));
  myFile.print(",");
  myFile.println(TE_measure(raw,3));
  myFile.close(); 
  Serial.println("Noted Down at");
  Serial.println(buf); 
  return 0;
  }   
}

int noteDown_ADC(){
   analogWrite(PUB_VCC,255);
  int16_t results;
  int16_t results_1;
  delay(50);
  //    Adafruit_ADS1115 ads1115_1(0x4a);
    //  ads1115_1.begin();
   //   results_1=-ads1115_1.readADC_Differential_0_1();
      Adafruit_ADS1115 ads1115(0x48);
      ads1115.begin();
      results=-ads1115.readADC_Differential_0_1();
  analogWrite(PUB_VCC,0);
   
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.day(),
           now.hour(), now.minute());


  myFile = SD.open("ADC_DATA.txt", FILE_WRITE);
  if (myFile){
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(buf);
  myFile.print(",");
  myFile.print(results);
  myFile.print(",");
  myFile.println(results_1);
  myFile.close(); 
  Serial.println("Noted Down at");
  Serial.print(buf); 
  Serial.println(results);
  return 0;
  }   
}

void noteShow(char* fname){
   myFile=SD.open(fname);
   while(myFile.available()){
     Serial.write(myFile.read());
   }
   myFile.close(); 
   return;
}

//********************5TE
float TE_measure(byte* raw,int option){
  float mos=0;
  float cond=0;
  float temp=0;
  int i=1;
  while (raw[i]!=' '){
       mos=mos*10+raw[i]-'0';
       i++;
     }
  mos=mos/50; 
  //moisture
  
  i++;
  while (raw[i]!=' '){
       cond=cond*10+raw[i]-'0';
       i++;
     }
  cond=cond/100;
  //conductivity
  
  i++;
  while (raw[i]!=0x0d){
       temp=temp*10+raw[i]-'0';
       i++;
     }
  temp=(temp-400)/10;
  switch (option){
    case 1:
      return mos;
    case 2:
      return cond;
    case 3:
      return temp;
    default:
      return 0;
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

//****************************setup*******************
void setup(void){
  Serial.begin(9600);
  int read_i=0;
  EE_ini_();
  char buf[5];
  pinMode(13,OUTPUT);
  pinMode(PUB_VCC,OUTPUT);  //other POWER
  pinMode(CLK_VCC,OUTPUT);  //Clock POWER
  pinMode(TE_IN,INPUT);
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
 // analogWrite(CLK_VCC,255);
  rtc.begin();
  now = rtc.now();
   if(now.minute()==30){
 //if(1==1){
  noteDown_5TE();
  noteDown_ADC();
  
//  noteShow("5TE_DATA.TXT");
 // noteShow("ADC_DATA.TXT");
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

