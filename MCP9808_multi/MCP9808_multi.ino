/**************************************************************************/
/*!
This is a demo for the Adafruit MCP9808 breakout
----> http://www.adafruit.com/products/1782
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
*/
/**************************************************************************/

#include <Wire.h>
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
//Adafruit_MCP9808 tempsensor19= Adafruit_MCP9808();
//Adafruit_MCP9808 tempsensor1A= Adafruit_MCP9808();
//Adafruit_MCP9808 *tempsensor;
//tempsensor= tempsensor18;
//tempsensor[1]= &tempsensor19;
//tempsensor[2]= &tempsensor1A;

void setup() {
  Serial.begin(9600);
  Serial.println("MCP9808 demo");
  
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808 at addr=0x18!");
    while (1);
  }
}

void loop(){
  temp9808_read(0);
  delay(1000);
  temp9808_read(1);
  delay(1000);
  temp9808_read(2);
  delay(1000);
  Serial.print('\n');
}



void temp9808_read(int choice) {
  // Read and print out the temperature, then convert to *F
  
  tempsensor.begin(0x18+choice);
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp of "); Serial.print(18+choice);Serial.print(" ");Serial.print(c); Serial.print("*C\t"); 
  Serial.print(f); Serial.println("*F");
  delay(250);
}
