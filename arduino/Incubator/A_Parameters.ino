

#define PARAM_TEMP_EXT_1    0
#define PARAM_TEMP_EXT_2    1
#define PARAM_TEMP_TARGET   2
#define PARAM_HBRIDGE_PID   3

#define PARAM_TEMP_PCB      8


#define PARAM_TEMP_TARGET_1  10
#define PARAM_TIME_1         11
#define PARAM_TEMP_TARGET_2  12
#define PARAM_TIME_2         13
#define PARAM_TEMP_TARGET_3  14
#define PARAM_TIME_3         15


#define PARAM_FLAGS           17 
#define PARAM_FLAG_INVERT_ROTARY 0  // invert rotary direction

#define PARAM_CURRENT_TIME   20
#define PARAM_STATE          21 // current state

#define STATE_OFF        0
#define STATE_CONSTANT   1
#define STATE_PROGRAM    2

#define PARAM_MENU          22 // current menu

/******************
   FLAG DEFINITION
 ******************/
#define PARAM_ENABLED      23
#define FLAG_FAN_EXTERNAL_CONTROL     0
#define FLAG_FAN_INTERNAL_CONTROL     1
#define FLAG_PID_CONTROL              2

#define PARAM_ERROR        24
#define FLAG_TEMP_PCB_PROBE_ERROR        0   // pcb probe failed (one wire not answering)
#define FLAG_TEMP_EXT_1_PROBE_ERROR      1   // ext probe 1 failed (one wire not answering)
#define FLAG_TEMP_EXT_2_PROBE_ERROR      2   // ext probe 2 failed (one wire not answering)

#define FLAG_TEMP_PCB_RANGE_ERROR        3   // temperature of pcb is outside range
#define FLAG_TEMP_EXT_1_RANGE_ERROR      4   // temperature of liquid is outside range
#define FLAG_TEMP_EXT_2_RANGE_ERROR      5   // temperature of liquid is outside range
#define FLAG_TEMP_TARGET_RANGE_ERROR     6   // target temperature is outside range

#define PARAM_STATUS             25  // current status




// value that should not be taken into account
// in case of error the parameter is set to this value
#define ERROR_VALUE  -32768

void resetParameters() {
  setAndSaveParameter(PARAM_TEMP_TARGET, 20);
  setAndSaveParameter(PARAM_TEMP_TARGET_1, 20);
  setAndSaveParameter(PARAM_TIME_1, 30);
  setAndSaveParameter(PARAM_TEMP_TARGET_2, 20);
  setAndSaveParameter(PARAM_TIME_2, 30);
  setAndSaveParameter(PARAM_TEMP_TARGET_3, 20);
  setAndSaveParameter(PARAM_TIME_3, 30);

  setAndSaveParameter(PARAM_FLAGS, 0);
  setAndSaveParameter(PARAM_MENU, 0);
  setAndSaveParameter(PARAM_ERROR, 0);
  setAndSaveParameter(PARAM_STATUS, 0);
  setAndSaveParameter(PARAM_ENABLED, 7);

  setQualifier(21313);
}


void checkParameters()
{
  if (getParameter(PARAM_STATE) < 0)
  {
    resetParameters();
  }
}

boolean isError(int filter) {
  if ((getParameter(PARAM_ERROR) & filter) != 0) return true;
  return false;
}

boolean isError() {
  return (getParameter(PARAM_ERROR) != 0);
}


// this method will check if there was a change in the error status and log it in this case
boolean saveAndLogError(boolean isError, byte errorFlag) {
  if (isError) {
    if (setParameterBit(PARAM_ERROR, errorFlag)) { // the status has changed
#ifdef EVENT_LOGGING
      writeLog(EVENT_ERROR_FAILED, errorFlag);
#endif
      return true;
    }
  } else {
    if (clearParameterBit(PARAM_ERROR, errorFlag)) { // the status has changed
#ifdef EVENT_LOGGING
      writeLog(EVENT_ERROR_RECOVER, errorFlag);
#endif
      return true;
    }
  }
  return false;
}
