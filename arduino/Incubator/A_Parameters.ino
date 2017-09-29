#define MAX_PARAM 26   // If the MAX_PARAM change you need to change the pointer in the EEPROM

#define EE_START_PARAM           0 // We save the parameter from byte 0 of EEPROM
#define EE_LAST_PARAM            (MAX_PARAM*2-1) // The last parameter is stored at byte 50-51

#define EE_QUALIFIER             (MAX_PARAM*2)

#define EEPROM_MIN_ADDR            0
#define EEPROM_MAX_ADDR          511


#define PARAM_TEMP_EXT_1    0
#define PARAM_TEMP_EXT_2    1
#define PARAM_TEMP_PCB      2
#define PARAM_TEMP_TARGET   3
#define PARAM_HBRIDGE_PID   4

#define PARAM_TEMP_TARGET_1  10
#define PARAM_TIME_1         11
#define PARAM_TEMP_TARGET_2  12
#define PARAM_TIME_2         13
#define PARAM_TEMP_TARGET_3  14
#define PARAM_TIME_3         15

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

  setAndSaveParameter(PARAM_MENU, 0);
  setAndSaveParameter(PARAM_ERROR, 0);
  setAndSaveParameter(PARAM_STATUS, 0);
  setAndSaveParameter(PARAM_ENABLED, 7);

  setQualifier(21313);
}



