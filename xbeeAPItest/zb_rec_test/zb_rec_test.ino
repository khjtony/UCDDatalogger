#include <CBeeArduinoLib.h>


//should receive as
//7e 00 1d 90 00 13 a2 00 40 91 c4 e7 71 74 01 6d 65 73 73 61 67 65 20 73 65 6e 74 20 74 65 73 74 b9
//7E 80 47 30 A0 B2 1A 10 2A 23 E7 2E 57 50 AB 6B CD C2 67 B1 90 AE CB 8B 81 BA 59 2E 57 0



//SoftwareSerial mySerial(10, 11); // RX, TX
byte income_byte[0x40];
char outcome_char[0x40];
int REC_FLAG=0;
int next_state=0;
int i=0;




void setup(){
  Serial.begin(115200);
}


void loop(){
  switch (next_state){
  case 0:
    if (Serial.available()>0){
      next_state=1;
    }
    if (REC_FLAG==1){
      next_state=2;
    }
    delay(10);
    break;
  case 1:
    income_byte[i]=Serial.read();
 //   Serial.print(income_byte[i]);
 //   Serial.print(' ');
    i=i+1;
    if (Serial.available()<=0){
      REC_FLAG=1;
      i=0;
      next_state=2;
    }
    next_state=0;
    break;
  case 2:
    zb_rec_pkt(income_byte,outcome_char);
    Serial.print(outcome_char);
   // Serial.write(income_byte,32);
    Serial.print('\n');
    REC_FLAG=0;
    next_state=0;
    break;
  default:
    next_state=0;
    break;
  }
}





