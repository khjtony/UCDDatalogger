#include <avr/sleep.h>
#include <avr/power.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <stdio.h>
#include <Wire.h>
#define WakePin  2
#include "kLogCollection.h"


void setup(){
  Serial.begin(115200);
}

void loop(){
  delay(1000);
}
