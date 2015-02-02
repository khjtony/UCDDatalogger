int analogy_2=2;
int analogy_4=4;
float v2=0;
float v4=0;
float val=0;

void setup(){
  Serial.begin(19200);
}

void loop(){
  v2=analogRead(analogy_2);
  v2=(v2/1024)*5;
  v4=analogRead(analogy_4);
  v4=(v4/1024)*5;
  
  val=v2-v4;
  Serial.print(v2);
  Serial.print(",");
  Serial.print(v4);
  Serial.print(",");
  val=val*1000;
  Serial.println(val);
  
  delay(500);
}
