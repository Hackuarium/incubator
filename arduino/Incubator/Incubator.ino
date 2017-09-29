#include <NilRTOS.h>
// Library that allows to start the watch dog allowing automatic reboot in case of crash
// The lowest priority thread should take care of the watch dog
#include <avr/wdt.h>

// git clone https://github.com/PaulStoffregen/Time
#include <TimeLib.h>


#define TEMPERATURE_EXT_1    A0
#define TEMPERATURE_EXT_2    A1
#define TEMPERATURE_PCB      A2
#define FAN_EXTERNAL         A3
#define FAN_INTERNAL         A4

#define HBRIDGE_INH  MISO
#define HBRIDGE_IN1  MOSI
#define HBRIDGE_IN2  SCK

#define THR_ERROR 13 // define the pin to blink if there is an error

void setup() {
  Serial.begin(9600);
  setupParameters();
  nilSysBegin();
}

void loop() {}


//Global Thread Locking
bool lockTimeCriticalZone = false;
void protectThread() {
  while (lockTimeCriticalZone) {
    nilThdSleepMilliseconds(5);
  }
  lockTimeCriticalZone = true;
}

void unprotectThread() {
  lockTimeCriticalZone = false;
}
