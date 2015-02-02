#include <Wire.h>
#include <Adafruit_ADS1015.h>
 
Adafruit_ADS1115 ads1115;
 Adafruit_ADS1115 ads1115_1(0x4A);
void setup(void)
{
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  
  pinMode(5,OUTPUT);
  analogWrite(5,255);
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
  ads1115.setGain(GAIN_TWO);
  ads1115.begin();
  ads1115_1.setGain(GAIN_TWO);
  ads1115_1.begin();
}
 
void loop(void)
{
  int16_t resultsA0;
  int16_t resultsA1;
   int16_t resultsB0;
  int16_t resultsB1;
  resultsA0 = ads1115.readADC_Differential_0_1();
  resultsA1 = ads1115.readADC_Differential_2_3();
  resultsB0 = ads1115_1.readADC_Differential_0_1();
  resultsB1 = ads1115_1.readADC_Differential_2_3();
  Serial.print("Differential A0: "); Serial.print(resultsA0); Serial.print("("); Serial.print(resultsA0 * 0.0625F); Serial.println("mV)");
  Serial.print("Differential A1: "); Serial.print(resultsA1); Serial.print("("); Serial.print(resultsA1 * 0.0625F); Serial.println("mV)");
  Serial.print("Differential B0: "); Serial.print(resultsB0); Serial.print("("); Serial.print(resultsB0 * 0.0625F); Serial.println("mV)");
  Serial.print("Differential B1: "); Serial.print(resultsB1); Serial.print("("); Serial.print(resultsB1 * 0.0625F); Serial.println("mV)");
  Serial.println("");
  delay(3000);
}
