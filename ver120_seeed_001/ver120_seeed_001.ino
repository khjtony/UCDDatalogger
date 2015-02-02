#include <EEPROM.h>
#include <SdFat.h>
#include <stdio.h>
#include <Wire.h>
#include "DS3231.h"
#include <SoftwareSerial.h>
#define TE_VCC    3
#define TE_IN      4

#include <Adafruit_ADS1015.h>

SoftwareSerial mySerial(TE_IN, 8); // RX, TX
DS3231 rtc;
Adafruit_ADS1115 ads1115;

#define error(s) sd.errorHalt_P(PSTR(s))
//***********************Wire setup**************************************
/*Clock
vcc--6
gnd--gnd
scl-scl
sda-sda


5TE
softseries
in--4
*/

//***********************s*************Macro definition*****************
#define SN_ADD1    (0x00)
#define SN1        (0x00)
#define SN_ADD2    (0x01)
#define SN2        (0x0f)
#define CLK_COUNT  (0x05)
#define CLK_INI    (0x10)
#define NOTE_FLAG  (0x15)

int BAT_0=A1;
int BAT_1=A2;

#define SD_CHIP_SELECT      10

// file system object
SdFat sd;
// text file for logging
SdFile myFile;
// Serial print stream
ArduinoOutStream cout(Serial);

DateTime now;
//************************************void EE_ini();
void EE_ini_(){
  EEPROM.write(SN_ADD1,SN1);
  EEPROM.write(SN_ADD2,SN2);
  EEPROM.write(NOTE_FLAG,0);
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
    Serial.write(raw[i]);
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
           
  if (!myFile.open("5TE_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening 5TE.txt for write failed");
  }
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
  
  Serial.println(TE_measure(raw,1));
  Serial.print(" ");
  Serial.println(TE_measure(raw,2));
  Serial.print(" ");
  Serial.println(TE_measure(raw,3));
  
  
  
  
  return 0;
  }   
//***********note down for ADC**************
int noteDown_ADC(){

  float multiplier = 0.1875F;
  int16_t results;
  int16_t results_1;
  delay(100);
  //    Adafruit_ADS1115 ads1115_1(0x4a);
    //  ads1115_1.begin();
   //   results_1=-ads1115_1.readADC_Differential_0_1();
      
      ads1115.begin();
      results=-ads1115.readADC_Differential_0_1();
   
  char timeBuf[50];
  snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.day(),
           now.hour(), now.minute());


  if (!myFile.open("ADC_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening ADC_DATA.txt for write failed");
  }
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(timeBuf); 
  myFile.print(results*multiplier);
  myFile.print(",");
  myFile.println(results_1*multiplier);
  myFile.close();
  
  Serial.println("Noted Down at");
  Serial.print(timeBuf); 
  Serial.print(results*multiplier);
  Serial.print(",");
  Serial.println(results_1*multiplier);
  
  return 0;
  }   

int noteDown_BAT(){
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.day(),
           now.hour(), now.minute());
  if (!myFile.open("BAT_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening BAT_DATA.txt for write failed");
  }
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(buf);
  myFile.print(",");
  myFile.println(bat_vol());
  myFile.close(); 
  Serial.println("BAT Noted Down at");
  Serial.println(buf); 
  return 0;
  }   


  
void noteShow(char* fname){
   int c;
   ifstream readFile(fname);
   while ((c = readFile.get()) >= 0) cout << (char)c;
   return;
}

//********************5TE data transfer
float TE_measure(byte* raw,int option){
  float mos=0;
  float cond=0;
  float temp=0;
  int i=1;  
  
  while (raw[i]>='9' || raw[i]<='0'){
    i++;
  }
 
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

//***************************Debug MODE*****************
//When SD does not insert
void debug(){
  Serial.println("DEBUG MODE ENABLED");
  return;
  
}

float bat_vol(){
  int bat_0=analogRead(BAT_0);
//  int bat_1=analogRead(BAT_1);
//  float bat=abs(bat_0-bat_1);
  float bat=bat_0*(10.0/1023.0);
  return bat;
}



//****************************setup*******************
void setup(void){   
  Serial.begin(9600);
  int read_i=0;
  EE_ini_();
  char buf[5];
  pinMode(TE_VCC,OUTPUT);   //Sensors Power
  analogWrite(TE_VCC,0);
  pinMode(10, OUTPUT);
  pinMode(BAT_0,INPUT);
  pinMode(BAT_1,INPUT);
  digitalWrite(BAT_0,HIGH);
  digitalWrite(BAT_1,HIGH);
  delay(200);
  Wire.begin();
  mySerial.begin(1200);
 if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) sd.initErrorHalt();
 //*********************read clock data from SD******
 if (sd.exists("CLOCK.txt")){
 myFile.open("CLOCK.txt");
  int i=0;
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
    Serial.println("Read clock end");
     EEPROM.write(CLK_COUNT,i);
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening CLOCK.txt");
  }
  }
//  analogWrite(CLK_VCC,0);
  
  


//*************************loop**************
void loop(){ 
  
  
  
//  analogWrite(CLK_VCC,255);
  delay(5150);
  now = rtc.now();
  //if (now.year()>2100){return;}
//  analogWrite(CLK_VCC,0);
  //if (chkClock()){

    
    if (1==1){
//   if((now.minute())==0 ){
 //     if (EEPROM.read(NOTE_FLAG)==0){
        noteDown_5TE();
        noteDown_ADC();
     //   noteDown_BAT();
      
  
        //  noteShow("5TE_DATA.TXT");
        // noteShow("ADC_DATA.TXT");
        //  noteShow("BATTERY.txt");
        EEPROM.write(NOTE_FLAG,1);
        delay(30);
    //  }    
  }
  else
  {
    EEPROM.write(NOTE_FLAG,0);
    Serial.println("Clock not reach or debug mode enabled");
    debug();
    Serial.println(bat_vol());
   // delay(1000);
  }  
}
