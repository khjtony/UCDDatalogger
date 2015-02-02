void setup(){
  Serial.begin(19200);
}

void loop(){
  int inbit;
  if(Serial.available()){
    while (inbit!='H'){
      inbit=Serial.read();
      Serial.println((char)inbit);
      delay(100);
    }
 // Serial.print(inByte);
  //Serial.println(inInt);
}
}
