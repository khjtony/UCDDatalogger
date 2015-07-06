#include <EEPROM.h>
#include <SdFat.h>
#include <stdio.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "DS3231.h"


#define RINGLIMIT 300
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

int msg_flag=0;

char ring[RINGLIMIT];
int ringindex=0;
int ringhead=0;

void setup(){
  Serial.begin(57600);
  
  //RTC 
  rtc.begin();

  //SD card 
  pinMode(SD_CHIP_SELECT,OUTPUT);
  if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) sd.initErrorHalt();
  //xbee
  pinMode(XBEE_EN,OUTPUT);
  digitalWrite(XBEE_EN,LOW);
  
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    if (inChar==0xfe || inChar==oxef){
      msg_flag++;
    }
    ring[ringindex]=inChar;
    if (ringindex>=RINGLIMIT-1){
      ringindex=0;
    }
  }
}


void loop(){
  int head=0;
  int tail=0;
  int i=0;
  char startchar;
  while(msg_flag>2){
    
  }
}



