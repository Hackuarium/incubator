#include <PID_v1.h>


void pid_ctrl();
heatingRegPID.SetOutputLimits(0, PID_OUTPUT_LIMIT);
heatingRegPID.SetMode(AUTOMATIC);      //turn the PID on, cf. PID library
heatingRegPID.SetSampleTime(950);      //set PID sampling time to 90ms
double heatingRegInput;
double heatingRegOutput;
double heatingRegSetpoint;
PID heatingRegPID(&heatingRegInput, &heatingRegOutput, &heatingRegSetpoint, 1, 0.0002, 5, DIRECT);


NIL_WORKING_AREA(waThreadTemperature, 128);
NIL_THREAD(ThreadTemperature, arg) {

  pinMode(FAN_1, OUTPUT);
  digitalWrite(FAN_1, HIGH);
  pinMode(FAN_2, OUTPUT);
  digitalWrite(FAN_2, HIGH);


  while (true) {
    heatingRegInput = getParameter(PARAM_TEMP_LIQ);
    heatingRegSetpoint = getParameter(PARAM_TEMP_TARGET);
    heatingRegPID.Compute();                                   // the computation takes only 30ms!
    setParameter(PARAM_TEMP_PID, heatingRegOutput);
    analogWrite(TEMP_PID, heatingRegOutput);

    nilThdSleepMilliseconds(500);
  }
}






