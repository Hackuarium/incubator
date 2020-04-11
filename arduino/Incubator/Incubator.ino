
#define THR_SERIAL      1


#include "A_Parameters.h"

#define THR_EEPROM_LOGGER 1
#define NUMBER_PARAMETERS_TO_LOG 6  // defines the number of parameters to log
// allowed values: 1, 2, 3, 4, 6, 8, 10, 14
// 2 first entries in the log will be logID and secodns between
#define LOG_INTERVAL 900   // we log every 15 minutes
// we can store 60 entries in the log
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

delay(500);
  
  setupParameters();
  checkParameters();
  nilSysBegin();
}

void loop() {}
