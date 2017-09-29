#ifdef THR_MONITORING
// Nice way to make some monitoring about activity. This should be the lower priority process
// If the led is "stable" (blinks 500 times per seconds) it means there are not too
// many activities on the microcontroler
NIL_WORKING_AREA(waThreadMonitoring, 0);
NIL_THREAD(ThreadMonitoring, arg) {
  // we activate the watchdog
  // we need to make a RESET all the time otherwise automatic reboot: wdt_reset();
  wdt_enable(WDTO_8S);

  byte turnOn = 0;
  pinMode(THR_MONITORING, OUTPUT);
  while (TRUE) {
    turnOn = ~turnOn;
    digitalWrite(THR_MONITORING, turnOn);
    nilThdSleepMilliseconds(250);
    if (getParameter(PARAM_STATUS) != 99) {
      wdt_reset();
    }
    else {
      nilThdSleepMilliseconds(750);
    }
  }
}
#endif


NIL_THREADS_TABLE_BEGIN()

NIL_THREADS_TABLE_ENTRY(NULL, ThreadSerial, NULL, waThreadSerial, sizeof(waThreadSerial))

#ifdef THR_LCD
NIL_THREADS_TABLE_ENTRY(NULL, ThreadLcd, NULL, waThreadLcd, sizeof(waThreadLcd))
#endif

#ifdef THR_ERROR
NIL_THREADS_TABLE_ENTRY(NULL, ThreadError, NULL, waThreadError, sizeof(waThreadError))
#endif

NIL_THREADS_TABLE_ENTRY(NULL, ThreadPID, NULL, waThreadPID, sizeof(waThreadPID))

NIL_THREADS_TABLE_ENTRY(NULL, ThreadTemperature, NULL, waThreadTemperature, sizeof(waThreadTemperature))



#ifdef THR_MONITORING
NIL_THREADS_TABLE_ENTRY(NULL, ThreadMonitoring, NULL, waThreadMonitoring, sizeof(waThreadMonitoring))
#endif


NIL_THREADS_TABLE_END()






