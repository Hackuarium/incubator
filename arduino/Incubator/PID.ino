#include "libino/PID/PID_v1.h"

#define PID_OUTPUT_LIMIT 255

void pid_ctrl();

double heatingRegInput;
double heatingRegOutput;
double heatingRegSetpoint;
PID heatingRegPID(&heatingRegInput, &heatingRegOutput, &heatingRegSetpoint, 1, 0.0002, 5, DIRECT);

NIL_WORKING_AREA(waThreadPID, 256);
NIL_THREAD(ThreadPID, arg) {

  heatingRegPID.SetOutputLimits(0, PID_OUTPUT_LIMIT);
  heatingRegPID.SetMode(AUTOMATIC);      //turn the PID on, cf. PID library
  heatingRegPID.SetSampleTime(950);      //set PID sampling time to 90ms

  pinMode(FAN_EXTERNAL, OUTPUT);
  pinMode(FAN_INTERNAL, OUTPUT);
  pinMode(HBRIDGE_IN1, OUTPUT);
  pinMode(HBRIDGE_IN2, OUTPUT);
  pinMode(HBRIDGE_INH, OUTPUT);


  while (true) {
    if (isError()) {
      digitalWrite(FAN_EXTERNAL, LOW);
      digitalWrite(FAN_INTERNAL, LOW);
      digitalWrite(HBRIDGE_INH, LOW);
    } else {
      if ( getParameterBit(PARAM_ENABLED,FLAG_FAN_EXTERNAL_CONTROL)) {
        digitalWrite(FAN_EXTERNAL, HIGH);
      } else {
        digitalWrite(FAN_EXTERNAL, LOW);
      }
      if ( getParameterBit(PARAM_ENABLED,FLAG_FAN_INTERNAL_CONTROL)) {
        digitalWrite(FAN_INTERNAL, HIGH);
      } else {
        digitalWrite(FAN_INTERNAL, LOW);
      }


      
      if ( getParameterBit(PARAM_ENABLED,FLAG_PID_CONTROL)) {
        digitalWrite(HBRIDGE_INH, HIGH);

        heatingRegInput = getParameter(PARAM_TEMP_EXT_1);
        heatingRegSetpoint = getParameter(PARAM_TEMP_TARGET);
        heatingRegPID.Compute();    // the computation takes only 30ms!

        setParameter(PARAM_HBRIDGE_PID, heatingRegOutput);
        analogWrite(HBRIDGE_IN1, heatingRegOutput);
      } else {
        digitalWrite(HBRIDGE_INH, LOW);
      }
    }
    nilThdSleepMilliseconds(500);
  }
}
