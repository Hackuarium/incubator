#include <NilRTOS.h>
// Library that allows to start the watch dog allowing automatic reboot in case of crash
// The lowest priority thread should take care of the watch dog
#include <avr/wdt.h>

// git clone https://github.com/PaulStoffregen/Time
#include <TimeLib.h>


#define TEMPERATURE_EXT_1    A0
#define TEMPERATURE_EXT_2    A1
#define TEMPERATURE_PCB      A2
#define FAN_1                A3
#define FAN_2                A4

void setup() {
  setupParameters();
  nilSysBegin();
}

void loop() {}

