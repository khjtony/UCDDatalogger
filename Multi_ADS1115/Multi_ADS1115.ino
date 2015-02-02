#include <Wire.h>
#include "DS3231.h"
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ADS1115A;
Adafruit_ADS1115 ADS1115B(0x4A);

#define PS_VCC    5
DS3231 rtc;
DateTime now;
//***********note down for ADC**************
int noteDown_ADC_cell(Adafruit_ADS1115* ads1115){

  float multiplier = 0.0625F;
  int16_t results;
  int16_t results_1;
  delay(100);
      ads1115->begin();
      results=ads1115->readADC_Differential_0_1();
      results_1=ads1115->readADC_Differential_2_3();
  Serial.print(",");
  Serial.print(results*multiplier);
  Serial.print(" ");
  Serial.print(results_1*multiplier);
 
  return 0;
  }   
 
 
int noteDown_ADC(){ 
  analogWrite(PS_VCC,255);
  delay(50);
  char timeBuf[50];
  snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d",
           
           now.year(), now.month(), now.date(),
           now.hour(), now.minute());



 
  
  Serial.print(timeBuf); 
  noteDown_ADC_cell(&ADS1115A);
  noteDown_ADC_cell(&ADS1115B);
  Serial.println("");

  analogWrite(PS_VCC,0);
 return 0;
 
}

void setup(){
  pinMode(PS_VCC,OUTPUT);
  analogWrite(PS_VCC,255);
Serial.begin(9600);
Wire.begin();
ADS1115A.setGain(GAIN_TWO);
ADS1115B.setGain(GAIN_TWO);
}


void loop(){
  noteDown_ADC();
  delay(5000);
} 
