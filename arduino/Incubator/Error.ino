// Thread displaying if there is an error

#ifdef THR_ERROR

NIL_WORKING_AREA(waThreadError, 32);
NIL_THREAD(ThreadError, arg) {
  pinMode(THR_ERROR, OUTPUT);
  while (TRUE) {
    if (isError()) {
      digitalWrite(THR_ERROR, HIGH);
      nilThdSleepMilliseconds(750);
      digitalWrite(THR_ERROR, LOW);
      nilThdSleepMilliseconds(250);
    } else {
      digitalWrite(THR_ERROR, HIGH);
      nilThdSleepMilliseconds(1000);
    }
  }
}

#endif

