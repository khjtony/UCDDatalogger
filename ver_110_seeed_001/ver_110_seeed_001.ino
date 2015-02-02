#include <EEPROM.h>
#include <SD.h>
#include <Wire.h>
#include "DS3231.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 8); // RX, TX
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Adafruit_ADS1015.h>
DS3231 rtc;
Adafruit_ADS1115 ads1115;
//***********************Wire setup
/*

SD
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
#define NOTE_FLAG  (0x15)

//#define CLK_VCC    3
#define TE_IN      4
#define SEN_VCC    5
#define ADC_VCC    3
int BAT_0=A1;
int BAT_1=A2;
File myFile;
#define SD_SE      10
DateTime now;
//************************************void EE_ini();
void EE_ini_(){
  EEPROM.write(SN_ADD1,SN1);
  EEPROM.write(SN_ADD2,SN2);
  EEPROM.write(NOTE_FLAG,0);
//  EEPROM.write(RCD_COUNT,0);
}


//***********note down for ADC**************
int noteDown_ADC(){
  int16_t results;
  int16_t results_1;
  delay(100);
  ads1115.begin();
  results=-ads1115.readADC_Differential_0_1();
   
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
  Serial.println(buf); 
  Serial.print(results);
  Serial.println(results_1);
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


void debug(){
  Serial.println("DEBUG MODE ENABLED");
  return;
  
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
  EE_ini_();
  pinMode(10, OUTPUT);
  delay(200);
  Wire.begin();
  Serial.println("Hello");
  
}

//*************************loop**************
void loop(){ 
  
  
  
//  analogWrite(CLK_VCC,255);
  delay(150);

  now = rtc.now();
  //if (now.year()>2100){return;}
//  analogWrite(CLK_VCC,0);
  //if (chkClock()){

    
    if (1==1){
 //  if((now.minute())==0 ){
 //     if (EEPROM.read(NOTE_FLAG)==0){
   //     noteDown_5TE();
       noteDown_ADC();
 //       noteDown_BAT();
      
  
        //  noteShow("5TE_DATA.TXT");
         noteShow("ADC_DATA.TXT");
        //  noteShow("BATTERY.txt");
        EEPROM.write(NOTE_FLAG,1);
        delay(30);
    //  }    
  }
  else
  {
    EEPROM.write(NOTE_FLAG,0);
    Serial.println("Clock not reach or debug mode enabled");
   // delay(1000);
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

