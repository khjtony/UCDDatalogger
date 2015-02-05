
#include <SdFat.h>
#include <XBee.h>
#include <string.h>
#include <Wire.h>

#define SD_CHIP_SELECT 10

SdFat sd;
SdFile myLog;

/*
This example is for Series 2 XBee
 Sends a ZB TX request with the value of analogRead(pin5) and checks the status response for success
*/

// create the XBee object
XBee xbee = XBee();


uint8_t payload[50];
uint16_t dummyRead;

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int pin5 = 0;

int statusLed = 13;
int errorLed = 13;

void setup() {
  if (!sd.begin(10, SPI_HALF_SPEED)) sd.initErrorHalt();
   
 
  Serial.begin(19200);
  xbee.setSerial(Serial);
}

void uint8cpy(uint8_t* load,char* str){
  int len=strlen(str);
  for (int i=0;i<len;i++){
    load[i]=0xff;
  }
//  load[len]=(uint8_t)'\0';
}

void sendFile(char* fname){
  int c=0;
  int i=0;
  ifstream file(fname);
  while ((c = file.get()) >= 0) {
    if (c=='\n'){
      xbee.send(zbTx);
      i=0;
    }else{
      payload[i]=(uint8_t)c;
      i++;
    } 
  }
  file.close();
}



void loop() {  
   sendFile("ADC_DATA.txt");  

  // break down 10-bit reading into two bytes and place in payloa
  delay(200000);
}

