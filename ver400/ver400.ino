#include <avr/sleep.h>
#include <avr/power.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <stdio.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_ADS1015.h>
#include "DS3231.h"
#include <Adafruit_MCP9808.h>




#define WAKE_UP  2
#define PWR_BOOST 3
#define SOFT_TX 4
#define XBEE_EN 5
#define MOI_PWR 6
#define MUX_A 7
#define MUX_B 8
#define MUX_Y 9
#define SD_CHIP_SELECT 10

#define MYID_ADD    (0x00)
#define MYID        (0x1A)

#define TEMP_MASK (B1111)
#define MOI_MASK (B1111)
#define PRESSURE_MASK (B1111)

SoftwareSerial mySerial(MUX_Y,SOFT_TX);
Adafruit_ADS1115 ADS1115A(0x48);
Adafruit_ADS1115 ADS1115B(0x49);
Adafruit_MCP9808 tempSensor = Adafruit_MCP9808();

// file system object
SdFat sd;
// text file for logging
SdFile myFile;
// Serial print stream
ArduinoOutStream cout(Serial);
DS3231 rtc;
DateTime nowTime;
DateTime lastTime;
char timeBuf[15];


//************************************void EEPROM_init
void EEPROM_init(){
  EEPROM.write(MYID_ADD,MYID);
}



void sleepNow()         // here we put the arduino to sleep
{
   
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register

    attachInterrupt(0,INT0_ISR, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
 
}

void INT0_ISR()
{
  //Keep this as short as possible. Possibly avoid using function calls
    detachInterrupt(0); 
}


//**************************MCP9808 Temperature********
float _temp9808_read(int choice) {
  // Read and print out the temperature, then convert to *F
   if (!tempSensor.begin(0x18+choice)) {
    return -255;
  }
  float c = tempSensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  return f;
}

void noteDown_TEMP(){
  byte index=0;

   char fname[15];  
  snprintf(fname, sizeof(fname), "%02d%02d%02d_T.log",
           nowTime.month(), nowTime.date(), nowTime.year()%100);

  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println("SD temperature error");
    return; 
  }
  myFile.print(MYID,HEX);
  myFile.print(",");
  myFile.print(timeBuf); 
  myFile.print(",");
  for (index=0;index<4;index++){
      myFile.print(_temp9808_read(index));
      myFile.print(" ");
  }
  myFile.println("");
  myFile.close();
}


//**************************record battery for reference********
int noteDown_BAT(){

  float voltage;
  int BatteryValue;
  BatteryValue = analogRead(A7);
  voltage = BatteryValue * (1.1 / 1024)* (10+2)/2;  //Voltage devider

  char fname[15];  
  snprintf(fname, sizeof(fname), "%02d%02d%02d_B.log",
           nowTime.month(), nowTime.date(), nowTime.year()%100);
           
  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println("SD BAT error");
    return 1;
  }
  myFile.print(MYID,HEX);
  myFile.print(",");
  myFile.print(timeBuf); 
  myFile.print(",");
  myFile.print(voltage);
  myFile.println("");
  myFile.close();
  return 0;
}   

//**************************record pressure********
int noteDown_ADC_cell(Adafruit_ADS1115* ads1115){

  float multiplier = 0.0625F;
  int16_t results;
  int16_t results_1;
  delay(100);
      ads1115->begin();
      results=-ads1115->readADC_Differential_0_1();
      results_1=-ads1115->readADC_Differential_2_3();
  myFile.print(results*multiplier);
  myFile.print(" ");
  myFile.print(results_1*multiplier);
  return 0;
  }   
 
 
int noteDown_ADC(){ 
  digitalWrite(MOI_PWR,LOW);

  char fname[15];  
  snprintf(fname, sizeof(fname), "%02d%02d%02d_P.log",
           nowTime.month(), nowTime.date(), nowTime.year()%100);


  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println("SD pressure error");
    return 1;
  }
  myFile.print(MYID,HEX);
  myFile.print(",");
  myFile.print(timeBuf); 
  myFile.print(",");
  noteDown_ADC_cell(&ADS1115A);
  myFile.print(" ");
  noteDown_ADC_cell(&ADS1115B);
  myFile.println("");
  myFile.close();
  digitalWrite(MOI_PWR,LOW);
 return 0;
 
}


//**************************record moisture********
float TE_measure(byte* raw,int option){
  float mos=0;
  float cond=0;
  float temp=0;
  int i=0;
  while (raw[i]<'0' || raw[i]>'9'){
    i++;
  }
  while (raw[i]!=' '){
   //    Serial.println(raw[i]-'0');
       mos=mos*10+raw[i]-'0';
       i++;            
     }
  mos=mos/50.0;
  mos=4.3*pow(10,-6)*pow(mos,3)-5.5*pow(10,-4)*pow(mos,2)+2.92*pow(10,-2)*mos-5.3*pow(10,-2); 
  //moisture
  
  i++;
  while (raw[i]!=' '){
       cond=cond*10+raw[i]-'0';
       i++;
     }
  if(cond>700){
  cond=5*(cond-700)+700;
  }
  cond=cond/100;
  //conductivity
  
  i++;
  while (raw[i]!=0x0d){
       temp=temp*10+raw[i]-'0';
       i++;
     }
  if (temp>900){
  temp=5*(temp-900)+900;
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

int noteDown_5TE_cell(unsigned char number){
  switch (number){
    case 1:
      digitalWrite(MUX_B,LOW);
      digitalWrite(MUX_A,LOW);
      break;
    case 2:
      digitalWrite(MUX_B,LOW);
      digitalWrite(MUX_A,HIGH);
      break;
    case 3:
      digitalWrite(MUX_B,HIGH);
      digitalWrite(MUX_A,LOW);
      break;
    case 4:
      digitalWrite(MUX_B,HIGH);
      digitalWrite(MUX_A,HIGH);
      break;
  }

  mySerial.flush();
  mySerial.listen();
  byte income;
  int i=0;
  byte raw[20];
  digitalWrite(PWR_BOOST,HIGH);
  delay(200);
  while (mySerial.available()){
    income=mySerial.read();
    raw[i]=income;
    i++;
    if (income==0x0A){ 
      digitalWrite(PWR_BOOST,LOW);
      break;
    }
    
  }
  digitalWrite(PWR_BOOST,LOW);


  myFile.print(TE_measure(raw,1));
  myFile.print("-");
  myFile.print(TE_measure(raw,2));
  myFile.print("-");
  myFile.print(TE_measure(raw,3));
  
  return 0;
}


int noteDown_5TE(){
  char fname[15];  
  snprintf(fname, sizeof(fname), "%02d%02d%02d_M.log",
           nowTime.month(), nowTime.date(), nowTime.year()%100);


  if (!myFile.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println("SD moisture error");
    return 1;
  }
  myFile.print(MYID,HEX);
  myFile.print(",");
  myFile.print(timeBuf);
  myFile.print(",");
  
  noteDown_5TE_cell(1);
  myFile.print(" ");
  delay(500);
  noteDown_5TE_cell(2);
  myFile.print(" ");
  delay(500);
  noteDown_5TE_cell(3);
  myFile.print(" ");
  delay(500);
  noteDown_5TE_cell(4);
  myFile.print(" ");
  delay(500);

  myFile.println("");
  myFile.close();
  return 0;
  }   




//update time
void update_time(DateTime nowtime){
    snprintf(timeBuf, sizeof(timeBuf), "%02d%02d%04d %02d%02d",
           nowTime.month(), nowTime.date(), nowTime.year(), 
           nowTime.hour(), nowTime.minute());
}


//send to coordinator
void Xbee_send_helper(char fname[],char cata){
  if (!myFile.open(fname, O_READ)) {
    Serial.println("SD send error");
    return;
  }
  char outchar[200];
  while (myFile.fgets(outchar,199,"\n")!=0){
    Serial.print(0xfe);
    Serial.print(MYID);
    Serial.print(cata);
    Serial.print(outchar);
    Serial.print(0xef);
  }
  myFile.close();
  
  
}
void Xbee_send(){
  digitalWrite(XBEE_EN,HIGH);
  delay(10000);  //waiting for log into network
  char fname[15];  
  //pressure
  snprintf(fname, sizeof(fname), "%02d%02d%02d_P.log",
           lastTime.month(), lastTime.date(), lastTime.year()%100);
  if (sd.exists(fname)){
    Xbee_send_helper(fname,'P');
   }
  //moisture
  snprintf(fname, sizeof(fname), "%02d%02d%02d_M.log",
           lastTime.month(), lastTime.date(), lastTime.year()%100);
  if (sd.exists(fname)){
    Xbee_send_helper(fname,'M');
   }
  //temperature
  snprintf(fname, sizeof(fname), "%02d%02d%02d_T.log",
           lastTime.month(), lastTime.date(), lastTime.year()%100);
  if (sd.exists(fname)){
    Xbee_send_helper(fname,'T');
   }
   
   digitalWrite(XBEE_EN,LOW);
}




void setup(){
  Serial.begin(57600);
  //Serial.println("Hello World");
  EEPROM_init();
  //system 
  analogReference(INTERNAL); 
  //temperature 
  //moisture 
  pinMode(MUX_A,OUTPUT);
  digitalWrite(MUX_A,LOW);
  pinMode(MUX_B,OUTPUT);
  digitalWrite(MUX_B,LOW);
  pinMode(MUX_Y,INPUT);
  digitalWrite(MUX_Y,HIGH);

  pinMode(MOI_PWR,OUTPUT);
  digitalWrite(MOI_PWR,LOW);
  //pressure 
  //power supply 
  pinMode(PWR_BOOST,OUTPUT);
  digitalWrite(PWR_BOOST,LOW);
  //RTC 
  pinMode(WAKE_UP, INPUT);
  digitalWrite(WAKE_UP,HIGH);
  rtc.begin();
  rtc.enableInterrupts(EveryMinute);
  //SD card 
  pinMode(SD_CHIP_SELECT,OUTPUT);
  if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) sd.initErrorHalt();
  //xbee
  pinMode(XBEE_EN,OUTPUT);
  digitalWrite(XBEE_EN,LOW);
  
  delay(200);
  Wire.begin();
  mySerial.begin(1200);
  attachInterrupt(0,INT0_ISR, LOW); 

}

void loop(){
  //Serial.println("Entered loop");
  delay(50);  //waiting for MCU stable
  rtc.clearINTStatus(); 
  nowTime = rtc.now(); 
  update_time(nowTime);
  

//  if (nowTime.hour()==12){    //newDay
 //   lastTime=nowTime;
//    Xbee_send();
 // }




//Serial.println("Record BAT");
  noteDown_BAT();
  
  //Serial.println("boost voltage");
  digitalWrite(PWR_BOOST,HIGH);
  delay(50);
  //Serial.println("Record Temperature");
  noteDown_TEMP();
  //Serial.println("Record ADC");
  noteDown_ADC();
  digitalWrite(PWR_BOOST,LOW);
  delay(500);
 // Serial.println("Record 5TE");
  noteDown_5TE();

  
  delay(3000);
 // sleepNow(); 
}  
