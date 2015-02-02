#include<EEPROM.h>

//Default setup
const byte EE_ID=0xff;
byte ID;
byte pd;
const byte EE_pd=0x24;
const int ID_add=0;
const int pd_add=1;
int pd_time=0;
int sensor_2=2;
int sensor_4=4;


void setup(){
  Serial.begin(19200);
  ID=EEPROM.read(ID_add);
  if (ID<=0x0 || ID>=0xff){
    EEPROM.write(ID_add,EE_ID);
  }
  pd=EEPROM.read(pd_add);
  if (pd<=0x0 || pd>=0xff){
    EEPROM.write(pd_add,EE_pd);
  }
  
  
  
}

void loop(){
  if(Serial.available()){
    char seed=Serial.read();
    switch (seed){
      case  'H':
          Serial.println("H");
          break;
      case 'A':
          ID=EEPROM.read(ID_add);
          Serial.print("A");
          Serial.print(String(ID));
          Serial.println("H");
          break;
      case  'B':
          pd=EEPROM.read(pd_add);
          Serial.print("B");
          Serial.print(String(pd));
          Serial.println("H");
          break;
      case  'C':
          pd_time=0
          while (true){
            delay(100);
            seed=Serial.read();
            if (seed=='H'){break;}
            pd_time=pd_time*10+(seed-'0');
          }
          delay(100);
          Serial.print("Peroid time was set to ");
          Serial.println(pd_time);
          pd=pd_time;
          EEPROM.write(pd_add,pd);
          break;
      case  'D':
          
            while(true){
              int V2=analogRead(sensor_2);
              int V4=analogRead(sensor_4);
              V2=V2/1024*5;
              V4=V4/1024*5;
              Serial.print("Current pressure is ");
              Serial.println(V2-V4);
            }
          
          break;
          
     
  }
  }
 // Serial.flush();
}
