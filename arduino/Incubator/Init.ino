
void resetParameters() {
  setAndSaveParameter(PARAM_TEMP_TARGET, 2000);
  setAndSaveParameter(PARAM_TEMP_TARGET_1, 2000);
  setAndSaveParameter(PARAM_TIME_1, 2000);
  setAndSaveParameter(PARAM_TEMP_TARGET_2, 2000);
  setAndSaveParameter(PARAM_TIME_2, 2000);
  setAndSaveParameter(PARAM_TEMP_TARGET_3, 2000);
  setAndSaveParameter(PARAM_TIME_3, 2000);

  setAndSaveParameter(PARAM_FAN_EXTERNAL, 255);
  setAndSaveParameter(PARAM_FAN_INTERNAL, 255);

  setAndSaveParameter(PARAM_STATE, 0);
  setAndSaveParameter(PARAM_FLAGS, 0);
  setAndSaveParameter(PARAM_MENU, 0);
  setAndSaveParameter(PARAM_ERROR, 0);
  setAndSaveParameter(PARAM_STATUS, 0);
  setAndSaveParameter(PARAM_ENABLED, 7);

#ifdef THR_EEPROM_LOGGER
  formatLog();
#endif


  setQualifier(21313);
}


void checkParameters()
{
  if (getParameter(PARAM_STATE) < 0)
  {
    resetParameters();
  }
}
