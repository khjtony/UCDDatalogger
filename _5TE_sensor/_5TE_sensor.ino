/*
  Software serial multple serial test
 
 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.
 
 The circuit: 
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)
 
 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts, 
 so only the following can be used for RX: 
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
 
 Not all pins on the Leonardo support change interrupts, 
 so only the following can be used for RX: 
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 
 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example
 
 This example code is in the public domain.
 
 */
#include <SoftwareSerial.h>
#define TE_VCC 3
SoftwareSerial mySerial4(4, 3); // RX, TX
SoftwareSerial mySerial7(7, 3); // RX, TX
SoftwareSerial mySerial8(8, 3); // RX, TX
void setup()  
{
  pinMode(6,OUTPUT);
  analogWrite(6,0);
  pinMode(9,OUTPUT);
  analogWrite(9,0);
  pinMode(5,OUTPUT);
  analogWrite(5,0);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial4.begin(1200);
  mySerial7.begin(1200);
  mySerial8.begin(1200);
//  mySerial.println("Hello, world?");
  pinMode(4,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);
}
/*
float TE_mos(byte* raw){
  float mos;
  int i=1;
  while (raw[i]!=' '){
       mos=mos*10+raw[i]-'0';
       i++;
     }
  mos=mos/50;   
  return mos;
}

float TE_temp(byte* raw){
  int i=0;
  int roof=0;
  while (raw[roof]!='z'){
    roof++;}
    roof-=2;
  float temp=0;
  while (raw[roof]!=' '){
    
    temp=temp+pow(10,i)*(raw[roof]-'0');
  //  Serial.println(temp);
    i++;
    roof--;
  }
temp=(temp-400)/10;
  return temp;
}
*/


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
void loop() // run over and over
{ 
  Serial.print("Now #4:  ");
  TE_Read(&mySerial4,5); 
  Serial.print("Now #7:  ");
  TE_Read(&mySerial7,6); 
  Serial.print("Now #8:  ");
  TE_Read(&mySerial8,9);   
  delay(3000);
}
  
void TE_deoverflow(){
   mySerial4.listen();
   mySerial4.flush();
   mySerial7.listen();
   mySerial7.flush();
   mySerial8.listen();
   mySerial8.flush();

}


void TE_Read(SoftwareSerial* mySerial,int power){
//  TE_deoverflow();
  mySerial->listen();
  delay(100);
  byte income;
  byte raw[20];
  int mos=0;
  int i=0;
  analogWrite(power,255);
  delay(80);
  while (mySerial->available()){
    income=mySerial->read();
    raw[i]=income;
    
    i++;
    delay(20);
    //Serial.print(income);
   // if (income==0x0A || i==19){
   //   analogWrite(3,0);
   //   delay(1000);
   //   break;
    //}
    if (income==0x0A){
      i=1;
       Serial.print(TE_measure(raw,1));
       Serial.print("-");
       Serial.print(TE_measure(raw,2));
       Serial.print("-");
       Serial.print(TE_measure(raw,3));
       Serial.println("");
       analogWrite(power,0);
       delay(100);
       return;}
    
    }
analogWrite(power,0);
       delay(100);
       return;
  }
  

