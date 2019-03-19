#include <Rotary

.h>

#define ROT_A      0
#define ROT_B      1
#define ROT_PUSH   7


// Rotary encoder is wired with the common to ground and the two
// outputs to pins 2 and 3.
Rotary rotary = Rotary(ROT_A, ROT_B);

boolean accelerationMode = 0;


void rotate() {

  int increment = 0;

  byte direction = rotary.process();

  
  if (direction == DIR_CW) {
    increment = -1;
  } else if (direction == DIR_CCW) {
    increment = 1;
  }

  if (increment == 0) return;

  long unsigned current = millis();
  long unsigned diff = current - lastRotaryEvent;
  lastRotaryEvent = current;

  if (diff < 50) {
    accelerationMode++;
    if (accelerationMode < 5) return;
    if (accelerationMode > 20) accelerationMode = 20;
  } else {
    accelerationMode = 0;
  }

/*
  if (getParameterBit(PARAM_FLAGS, PARAM_FLAG_INVERT_ROTARY) == 1) {
    increment *= -1;
  }
*/
  if (accelerationMode > 4) {
    rotaryCounter += (increment * accelerationMode);
  } else {
    if (accelerationMode == 0) {
      rotaryCounter += increment;
    }
  }
}


void setupRotary() {
  attachInterrupt(digitalPinToInterrupt(ROT_A), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_B), rotate, CHANGE);
  pinMode(ROT_PUSH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ROT_PUSH), eventRotaryPressed, CHANGE);
}



void eventRotaryA() {
  int increment = digitalRead(ROT_B) * 2 - 1;
  long current = millis();
  long diff = current - lastRotaryEvent;
  lastRotaryEvent = current;
  if (diff < 15) return;
  if (diff < 50) {
    if (accelerationMode) {
      rotaryCounter -= (increment * 5);
    } else {
      accelerationMode = true;
      rotaryCounter -= increment;
    }
  } else {
    accelerationMode = false;
    rotaryCounter -= increment;
  }
}


boolean rotaryMayPress = true; // be sure to go through release. Seems to allow some deboucing

void eventRotaryPressed() {
  byte state = digitalRead(ROT_PUSH);
  if (state == 0) {
    if (rotaryMayPress) {
      rotaryPressed = true;
      rotaryMayPress = false;
    }
  } else {
    rotaryMayPress = true;
  }
}
