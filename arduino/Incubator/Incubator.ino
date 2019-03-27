
#define THR_SERIAL      1
#define MAX_PARAM       26
#include "libino/hack.h"


#define TEMPERATURE_EXT_1    A0
#define TEMPERATURE_EXT_2    A1
#define TEMPERATURE_PCB      A2
#define FAN_EXTERNAL         6
#define FAN_INTERNAL         11

#define HBRIDGE_INH  MISO
#define HBRIDGE_IN1  SCK
#define HBRIDGE_IN2  MOSI


#define THR_ERROR 13 // define the pin to blink if there is an error

void setup() {
  setupParameters();
   checkParameters(); 
  nilSysBegin();
}

void loop() {}
