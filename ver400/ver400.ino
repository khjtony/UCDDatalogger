#include <avr/sleep.h>
#include <avr/power.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <stdio.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "kLogCollection.h"

#define WakePin  2
#define SD_CHIP_SELECT      10
#define SN_ADD1    (0x00)
#define SN1        (0x00)
#define SN_ADD2    (0x01)
#define SN2        (0x0f)


// file system object
SdFat sd;
// text file for logging
SdFile myFile;
// Serial print stream
ArduinoOutStream cout(Serial);
DS3231 rtc;
DateTime nowTime;

kADS1115 pressure_sensor(0x1 | 0x1<<1);
kMCP9808 temperature_sensor(0x1 | 0x1<<1);
k5TE TE_sensor(9,3);

//************************************void EE_ini();
void EE_ini_(){
  EEPROM.write(SN_ADD1,SN1);
  EEPROM.write(SN_ADD2,SN2);
  EEPROM.write(NOTE_FLAG,0);
//  EEPROM.write(RCD_COUNT,0);
}



void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
 
    /* Now it is time to enable an interrupt. We do it here so an
     * accidentally pushed interrupt button doesn't interrupt
     * our running program. if you want to be able to run
     * interrupt code besides the sleep function, place it in
     * setup() for example.
     *
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
     *
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
 
    attachInterrupt(0,INT0_ISR, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
 
}

void INT0_ISR()
{
  //Keep this as short as possible. Possibly avoid using function calls
    detachInterrupt(0); 
}



void setup(){
  Serial.begin(57600);
  EE_ini_();
  pinMode(WakePin, INPUT);
  digitalWrite(WakePin,HIGH);
  analogReference(INTERNAL); 
  delay(200);
  rtc.begin();
  Wire.begin();
  attachInterrupt(0,INT0_ISR, LOW); 
}

void loop(){
  delay(50);  //waiting for MCU stable
  rtc.clearINTStatus(); 
  nowTime = rtc.now(); 
  if ((nowTime.hour() % 24) == ( SN1<<8 + SN2 ) % 24){
    upload_log(nowTime);
  }
  pressure_sensor.log(nowTime);
  temperature_sensor.log(nowTime);
  TE_sensor.log(nowTime);
  delay(30);
  sleepNow(); 
}  
