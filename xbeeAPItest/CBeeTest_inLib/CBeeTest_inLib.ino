#include <CBeeArduinoLib.h>

//7E 00 1F 10 01 00 00 00 00 00 00 00 00 FF FE 00 00 6D 65 73 73 61 67 65 20 73 65 6E 74 20 74 65 73 74 52

#include <stdint.h>


void setup(){
  Serial.begin(115200);
  delay(20000);
}

void loop(){
  
  byte addr64[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  byte addr16[2]={0xff,0xfe};
  uint8_t all_len;
  byte* all_char=zb_tx_rq(addr64,addr16,0x00,0x00,"message sent test",&all_len);
  uint8_t i=0;
  for(i=0;i<all_len;i++){
    Serial.print(all_char[i],HEX);
    Serial.print(' ');
    //Serial.write(all_char[i]);
   }
   
  delay(2000);
  while(Serial.available()>0){
    Serial.print(Serial.read());
  }
  

  delay(10000);
}

