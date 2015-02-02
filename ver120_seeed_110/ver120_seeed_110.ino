#include <avr/sleep.h>
#include <avr/power.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <stdio.h>
#include <Wire.h>
#include "DS3231.h"
#include <SoftwareSerial.h>
#define PS_VCC    5
#define WakePin  2
#include <Adafruit_ADS1015.h>


SoftwareSerial mySerial4(4, 3); // RX, TX
SoftwareSerial mySerial7(7, 3); // RX, TX
SoftwareSerial mySerial8(8, 3); // RX, TX
DS3231 rtc;
Adafruit_ADS1115 ADS1115A;
Adafruit_ADS1115 ADS1115B(0x4A);

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
TE_VCC    3
*/

//***********************s*************Macro definition*****************
#define SN_ADD1    (0x00)
#define SN1        (0x00)
#define SN_ADD2    (0x01)
#define SN2        (0x0f)
#define CLK_COUNT  (0x05)
#define CLK_INI    (0x10)
#define NOTE_FLAG  (0x15)

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


//********************5TE data transfer*******************
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

//***********note down for 5TE**************
int noteDown_5TE_cell(SoftwareSerial* mySerial,int power){
  mySerial->listen();
  byte income;
  int i=0;
  byte raw[20];
  delay(100);
  analogWrite(power,255);
  delay(80);
  while (mySerial->available()){
    income=mySerial->read();
    raw[i]=income;
    i++;
    if (income==0x0A){ 
      analogWrite(power,0);
      break;
    }
    
  }
  analogWrite(power,0);
   if (!myFile.open("5TE_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening 5TE.txt for write failed");
  }
  myFile.print(",");
  myFile.print(TE_measure(raw,1));
  myFile.print("-");
  myFile.print(TE_measure(raw,2));
  myFile.print("-");
  myFile.print(TE_measure(raw,3));
  myFile.close(); 
  return 0;
}
int noteDown_5TE(){
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.date(),
           now.hour(), now.minute());
  if (!myFile.open("5TE_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening 5TE.txt for write failed");
  }
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(buf);
  myFile.close();
  
  noteDown_5TE_cell(&mySerial4,5);
  noteDown_5TE_cell(&mySerial7,6);
  noteDown_5TE_cell(&mySerial8,9);
  
    if (!myFile.open("5TE_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening 5TE.txt for write failed");
  }
  myFile.println("");
  myFile.close();
  return 0;
  }   
//***********note down for ADC**************
int noteDown_ADC_cell(Adafruit_ADS1115* ads1115){

  float multiplier = 0.0625F;
  int16_t results;
  int16_t results_1;
  delay(100);
      ads1115->begin();
      results=-ads1115->readADC_Differential_0_1();
      results_1=-ads1115->readADC_Differential_2_3();
   if (!myFile.open("ADC_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening ADC_DATA.txt for write failed");
  }
  myFile.print(",");
  myFile.print(results*multiplier);
  myFile.print(",");
  myFile.print(results_1*multiplier);
  myFile.close();
  return 0;
  }   
 
 
int noteDown_ADC(){ 
  analogWrite(PS_VCC,255);
  char timeBuf[50];
  snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.date(),
           now.hour(), now.minute());


  if (!myFile.open("ADC_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening ADC_DATA.txt for write failed");
  }
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(timeBuf); 
  myFile.close();
  noteDown_ADC_cell(&ADS1115A);
  noteDown_ADC_cell(&ADS1115B);
  if (!myFile.open("ADC_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening 5TE.txt for write failed");
  }
  myFile.println("");
  myFile.close();
  analogWrite(PS_VCC,0);
 return 0;
 
}



int noteDown_BAT(){

  float voltage = get_Battery();
  char timeBuf[50];
  snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.date(),
           now.hour(), now.minute());


  if (!myFile.open("BAT_DATA.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening ADC_DATA.txt for write failed");
  }
  myFile.print(EEPROM.read(SN_ADD1),HEX);
  myFile.print(EEPROM.read(SN_ADD2),HEX);
  myFile.print(",");
  myFile.print(timeBuf); 
  myFile.print(",");
  myFile.print(voltage);
  myFile.println("");
  myFile.close();
  return 0;
  }   


  
void noteShow(char* fname){
   int c;
   ifstream readFile(fname);
   while ((c = readFile.get()) >= 0) cout << (char)c;
   return;
}

float get_Battery(){
  float voltage;
  int BatteryValue;
  BatteryValue = analogRead(A7);
  voltage = BatteryValue * (1.1 / 1024)* (10+2)/2;  //Voltage devider
  return voltage;
}

//***************************Debug MODE&Sleeping*****************
//When SD does not insert
void debug(){
  Serial.println("DEBUG MODE ENABLED");
  return;
  
}



void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
 
    /* Now it is time to enable an interrupt. We do it here so an
     * accidentally pushed interrupt button doesn't interrupt
     * our running program. if you want to be able to run
     * interrupt code besides the sleep function, place it in
     * setup() for example.
     *
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
     *
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
 
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
 //   EEPROM.write(NOTE_FLAG,0);
}


//****************************setup*******************
void setup(void){   
  Serial.begin(9600);
  Serial.println("hello");
  EE_ini_();
  pinMode(WakePin, INPUT);
  digitalWrite(WakePin,HIGH);
  analogReference(INTERNAL); 
  delay(200);
  rtc.begin();
  Wire.begin();
  attachInterrupt(0,INT0_ISR, LOW); 
  //5TE sensors power
  pinMode(5,OUTPUT);
  analogWrite(5,0);
  pinMode(6,OUTPUT);
  analogWrite(6,0);
  pinMode(9,OUTPUT);
  analogWrite(9,0);
  //5TE sensors communications
  mySerial4.begin(1200);
  mySerial7.begin(1200);
  mySerial8.begin(1200);
//  mySerial.println("Hello, world?");
  pinMode(4,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  //5TE sensors initialization end
  ADS1115A.setGain(GAIN_TWO);
  ADS1115B.setGain(GAIN_TWO);
  //Enable Interrupt 
  rtc.enableInterrupts(EveryHour); //interrupt at  EverySecond, EveryMinute, EveryHour
     
 if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) sd.initErrorHalt();
   }

  


//*************************loop**************
void loop(){ 
  delay(50);  //waiting for MCU stable
  rtc.clearINTStatus(); 
 // Serial.println("DAY");
  now = rtc.now(); 
   // if (1==1){
   //   if (EEPROM.read(NOTE_FLAG)==0){
        noteDown_5TE();
        noteDown_ADC();
        noteDown_BAT();
     //   Serial.println("HAHA");  
        //  noteShow("5TE_DATA.TXT");
        // noteShow("ADC_DATA.TXT");
        //  noteShow("BATTERY.txt");
     //   EEPROM.write(NOTE_FLAG,1);
        delay(30);
       sleepNow(); 
  }  

