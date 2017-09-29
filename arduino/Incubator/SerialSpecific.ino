void processSpecificCommand(char* data, char* paramValue, Print* output) {


  switch (data[0]) {
    case 'c':
      setParameter(PARAM_STATUS, STATE_CONSTANT);
      break;
    case 'd':
      setParameter(PARAM_STATUS, STATE_OFF);
      break;
    case 'p':
      setParameter(PARAM_STATUS, STATE_PROGRAM);
      setParameter(PARAM_CURRENT_TIME, 0);
      break;

  }
}

void printSpecificHelp(Print * output) {
  output->println(F("(c)onstant"));
  output->println(F("(d)isable"));
  output->println(F("(p)rogram"));
}

