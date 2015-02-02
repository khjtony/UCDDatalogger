#include <CBeeArduinoLib.h>

//         7E 00 23 10 01 00 00 00 00 00 00 00 00 FF FE 00 00 6D 65 73 73 61 67 65 20 73 65 6E 74 20 74 65 73 74 52
//17 31 35 7E 00 1F 10 01 00 00 00 00 00 00 00 00 FF FE 00 00 6D 65 73 73 61 67 65 20 73 65 6E 74 20 74 65 73 74 52 
//7E 03 00 10 01 00 00 00 00 00 00 00 00 00 00 00 00 49 20 61 6D 20 73 75 72 65 20 74 68 69 73 20 6D 73 67 20 63 61 6E 20 62 65 20 73 65 6E 74 03 
//0x7E 0x00 0x16 0x10 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFF 0xFE 0x00 0x00 0x54
//0x78 032 0x43 0x6F 0x6F 0x72 0x64 0xFC
//7e 00 16 10 01 00 00 00 00 00 00 00 00 ff fe 00 00 54 78 32 43 6f 6f 72 64 fc

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
    //Serial.print(all_char[i],HEX);
    //Serial.print(' ');
    Serial.write(all_char[i]);
   }
  while (Serial.available()>0){
    Serial.print(Serial.read());
  }

  delay(10000);
}

