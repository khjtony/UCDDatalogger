#include <SdFat.h>
#include <XBee.h>
#include <string.h>
#include "DS3231.h"
#include <stdio.h>
#include <Wire.h>

#define SD_CHIP_SELECT 10

DS3231 rtc;
SdFat sd;
SdFile myLog;
DateTime now;
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();
uint8_t inCome[80];
int statusLed = 13;

//********************************begin**************************
void flashLed(int pin, int times, int wait) {
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}

void errorLog(String msg){
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d",
           now.year(), now.month(), now.date(),
           now.hour(), now.minute());
    if (!myLog.open("errorLOG.log", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening 5TE.txt for write failed");
  }
  myLog.print(buf);
  myLog.print(msg);
  myLog.println("");
  myLog.close();
  return;
}

void normalLog(){
  char fname[11];
  for (int i=0;i<5;i++){
    fname[i]=(uint8_t)rx.getData(i);
  }
  fname[6]='.';
  fname[7]='l';
  fname[8]='o';
  fname[9]='g';
  fname[10]='\0';
  
  if (!myLog.open(fname, O_RDWR | O_CREAT | O_AT_END)) {
    return;
  }
  
  myLog.println((char*)rx.getData());
  myLog.close();
}


//***************************************util**********************

void setup() {
  // start serial
  Serial.begin(19200);
  xbee.begin(Serial);
  
  flashLed(statusLed, 3, 50);
}


void loop() {
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
            
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            // the sender got an ACK
            flashLed(statusLed, 10, 10);
        } else {
            // we got it (obviously) but sender didn't get an ACK
            flashLed(statusLed, 2, 20);
        }
        // set dataLed PWM to value of the first byte in the data
//        analogWrite(dataLed, rx.getData(0));
          normalLog();
      }else if (xbee.getResponse().isError()) {
          errorLog("Bad Response");
      
    }
}
}
