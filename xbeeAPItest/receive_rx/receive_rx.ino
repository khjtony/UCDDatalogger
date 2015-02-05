#include <SoftwareSerial.h>

#include <string.h>

SoftwareSerial mySerial(5,6);

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(115200);
  mySerial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  // print the string when a newline arrives:
  int pos=inputString.indexOf(0x7e,10);
  String tempString=inputString.substring(0,pos);
  //inputString.remove(pos,inputString.length());
  //Serial.println("");
  //Serial.print(inputString[inputString.length()-3]);
  //Serial.print(inputString[inputString.length()-2]);
  mySerial.print(inputString.length());
  mySerial.println("");
  delay(500);
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    if (inChar==0x7e){
      inputString+=' ';
    }
    inputString += inChar;
  }
}
