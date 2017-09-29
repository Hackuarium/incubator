#define THR_LCD 1

#ifdef THR_LCD


#include <LiquidCrystal.h>

#define LCD_E      6
#define LCD_RS     12
#define LCD_D4     8
#define LCD_D5     9
#define LCD_D6     10
#define LCD_D7     5
#define LCD_BL     13    // back light
#define LCD_VO     11

#define LCD_NB_ROWS     2
#define LCD_NB_COLUMNS  16

#define ROT_A      0
#define ROT_B      1
#define ROT_PUSH   7

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


boolean rotaryPressed = false;
int rotaryCounter = 0;
boolean captureCounter = false; // use when you need to setup a parameter from the menu
long lastRotaryEvent = millis();



NIL_WORKING_AREA(waThreadLcd, 192);
NIL_THREAD(ThreadLcd, arg) {
  // initialize the library with the numbers of the interface pins

  setupRotary();
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH); // backlight
  nilThdSleepMilliseconds(10);
  lcd.begin(LCD_NB_COLUMNS, LCD_NB_ROWS);

  setParameter(PARAM_MENU, 0);

  while (true) {
    lcdMenu();
    nilThdSleepMilliseconds(40);
  }
}


byte noEventCounter = 0;
byte previousMenu = 0;

void lcdMenu() {
  byte currentMenu = getParameter(PARAM_MENU);
  if (previousMenu != currentMenu) { // this is used to clear screen from external process for example
    noEventCounter = 0;
    previousMenu = currentMenu;
  }
  if (rotaryCounter == 0 && ! rotaryPressed) {
    if (noEventCounter < 255) noEventCounter++;
  } else {
    noEventCounter = 0;
  }
  if (noEventCounter > 250 && getParameter(PARAM_STATUS) == 0) {
    if (currentMenu - currentMenu % 10 != 20) currentMenu = 20;
    captureCounter = false;
    noEventCounter = 0;
  }
  boolean doAction = rotaryPressed;
  rotaryPressed = false;
  int counter = rotaryCounter;
  rotaryCounter = 0;
  switch (currentMenu < 100 ? currentMenu - currentMenu % 10 : currentMenu - currentMenu % 50) {
    case 0:
      lcdMenuHome(counter, doAction);
      break;
    case 10:
      lcdMenuSettings(counter, doAction);
      break;
    case 20:
      lcdDefault(counter, doAction);
      break;
    case 40:
      lcdUtilities(counter, doAction);
      break;
    case 100:
      lcdResults(counter, doAction);
      break;
  }
}

void lcdDefault(int counter, boolean doAction) {
  if (doAction) setParameter(PARAM_MENU, 0);
  updateCurrentMenu(counter, 6);
  if (noEventCounter < 2) lcd.clear();
  byte menu = getParameter(PARAM_MENU) % 10;
  if (menu < 5) {
    lcd.setCursor(0, 0);
    lcdPrintBlank(2);
  } else {
    lcd.setCursor(0, 0);
    epochToString(now(), &lcd);
    lcd.setCursor(6, 1);
    lcd.print("s:");
    lcd.print(millis() / 1000);
  }
}



void lcdNumberLine(byte line) {
  lcd.print(getParameter(PARAM_MENU) % 10 + line + 1);
  if (line == 0) {
    lcd.print(".*");
  } else {
    lcd.print(". ");
  }
}

void updateCurrentMenu(int counter, byte maxValue) {
  updateCurrentMenu(counter, maxValue, 10);
}

void updateCurrentMenu(int counter, byte maxValue, byte modulo) {
  byte currentMenu = getParameter(PARAM_MENU);
  if (captureCounter) return;
  if (counter < 0) {
    setParameter(PARAM_MENU, currentMenu + max(counter, - currentMenu % modulo));
  } else if (counter > 0) {
    setParameter(PARAM_MENU, currentMenu + min(counter, maxValue - currentMenu % modulo));
  }
}

void lcdMenuHome(int counter, boolean doAction) {
  if (noEventCounter > 2) return;
  lcd.clear();
  byte lastMenu = 5;
  updateCurrentMenu(counter, lastMenu);

  for (byte line = 0; line < LCD_NB_ROWS; line++) {
    lcd.setCursor(0, line);
    if ( getParameter(PARAM_MENU) % 10 + line <= lastMenu) lcdNumberLine(line);



    switch (getParameter(PARAM_MENU) % 10 + line) {
      case 0:
        lcd.print(F("Stop control"));
        if (doAction) {
          setParameter(PARAM_STATUS, STATE_OFF);
        }
        break;
      case 1:
        lcd.print(F("Constant temp."));
        if (doAction) {
          setParameter(PARAM_STATUS, STATE_CONSTANT);
        }
        break;
      case 2:
        lcd.print(F("Temp. program"));
        if (doAction) {
          setParameter(PARAM_STATUS, STATE_PROGRAM);
          setParameter(PARAM_CURRENT_TIME, 0);
        }
        break;
      case 3:
        lcd.print(F("Settings"));
        if (doAction) {
          setParameter(PARAM_MENU, 10);
        }
        break;
      case 4:
        lcd.print(F("Status"));
        if (doAction) {
          setParameter(PARAM_MENU, 20);
        }
        break;
      case 5:
        lcd.print(F("Utilities"));
        if (doAction) {
          setParameter(PARAM_MENU, 40);
        }
        break;
    }
    doAction = false;
  }
}

void lcdUtilities(int counter, boolean doAction) {
  if (noEventCounter > 2) return;
  lcd.clear();
  byte lastMenu = 2;
  updateCurrentMenu(counter, lastMenu);

  for (byte line = 0; line < LCD_NB_ROWS; line++) {
    lcd.setCursor(0, line);
    if ( getParameter(PARAM_MENU) % 10 + line <= lastMenu) lcdNumberLine(line);

    switch (getParameter(PARAM_MENU) % 10 + line) {
      case 0:
        lcd.print(F("Option 1"));
        if (doAction) {

        }
        break;
      case 1:
        lcd.print(F("Option 2"));
        if (doAction) {

        }
        break;
      case 2:
        lcd.print(F("Reset parameters"));
        if (doAction) {
          resetParameters();
          setParameter(PARAM_MENU, 20);
        }
        break;

    }
    doAction = false;
  }
}


void lcdMenuSettings(int counter, boolean doAction) {

  byte lastMenu = 7;
  if (! captureCounter) updateCurrentMenu(counter, lastMenu);

  byte currentParameter = 0;
  float currentFactor = 1;
  byte parameterNumber = 0;
  char currentUnit[5] = "\0";
  int maxValue = 32767;
  int minValue = -32768;

  lcd.clear();

  switch (getParameter(PARAM_MENU) % 10) {
    case 0:
      lcd.print(F("Default temp."));
      currentParameter = PARAM_TEMP_TARGET;
      minValue = -10;
      maxValue = 50;
      strcpy(currentUnit, "\xDF\x43");
      break;
    case 1:
      lcd.print(F("Temperature 1"));
      currentParameter = PARAM_TEMP_TARGET_1;
      minValue = -10;
      maxValue = 50;
     strcpy(currentUnit, "\xDF\x43");
      break;
    case 2:
      lcd.print(F("Wating time 1"));
      currentParameter = PARAM_TIME_1;
      minValue = 0;
      maxValue = 10000;
      strcpy(currentUnit, "min.\0");
      break;
    case 3:
      lcd.print(F("Temperature 2"));
      currentParameter = PARAM_TEMP_TARGET_2;
      minValue = -10;
      maxValue = 50;
     strcpy(currentUnit, "\xDF\x43");
      break;
    case 4:
      lcd.print(F("Waiting time 2"));
      currentParameter = PARAM_TIME_2;
      minValue = 0;
      maxValue = 10000;
      strcpy(currentUnit, "min.\0");
      break;
    case 5:
      lcd.print(F("Temperature 3"));
      currentParameter = PARAM_TEMP_TARGET_3;
      minValue = -10;
      maxValue = 50;
      strcpy(currentUnit, "\xDF\x43");
      break;
    case 6:
      lcd.print(F("Waiting time 3"));
      currentParameter = PARAM_TIME_3;
      minValue = 0;
      maxValue = 10000;
      strcpy(currentUnit, "min\0");
      break;
    case 7:
      lcd.print(F("Main menu"));
      if (doAction) {
        setParameter(PARAM_MENU, 1);
      }
      return;
  }

  if (doAction) {
    captureCounter = ! captureCounter;
    if (! captureCounter) {
      setAndSaveParameter(currentParameter, getParameter(currentParameter));
    }
  }
  if (captureCounter) {
    int newValue = getParameter(currentParameter) + counter;
    setParameter(currentParameter, max(min(maxValue, newValue), minValue));
  }

  lcd.setCursor(0, 1);
  if (millis() % 1000 < 500 && captureCounter) {
    lcd.print((char)255);
  } else {
    lcd.print(" ");
  }
  switch (getParameter(PARAM_MENU) % 10) {
    default:
      if (currentFactor == 1) {
        lcd.print((getParameter(currentParameter)));
      } else {
        lcd.print(((float)getParameter(currentParameter))*currentFactor);
      }
      lcd.print(" ");
      lcd.print(currentUnit);
  }
}


void lcdResults(int counter, boolean doAction) {
  if (doAction) setParameter(PARAM_MENU, 0);
  if (noEventCounter < 2) lcd.clear();

  // calculate the last experiment based on epoch of each experiment
  /*
    byte lastExperiment = 1;
    for (lastExperiment; lastExperiment < MAX_EXPERIMENTS; lastExperiment++) {
    if (data[lastExperiment * 6] <= data[0]) break;
    }

    updateCurrentMenu(counter, lastExperiment - 1, 50);
    byte start = getParameter(PARAM_MENU) % 50;
    for (byte i = start; i < min(lastExperiment, start + LCD_NB_ROWS); i++) {
    lcd.setCursor(0, i - start);
    lcd.print((data[i * 6] - data[0]) / 1000);
    lcd.print(" ");
    if (data[getParameter(PARAM_COLOR)] == LONG_MAX_VALUE || data[i * 6 + getParameter(PARAM_COLOR)] == LONG_MAX_VALUE) {
    lcd.print(F("OVER"));
    } else {
    lcd.print(log10((double)data[getParameter(PARAM_COLOR)] / (double)data[i * 6 + getParameter(PARAM_COLOR)]));
    }
    lcd.print(" ");
    lcd.print(data[i * 6 + getParameter(PARAM_COLOR)]);
    lcdPrintBlank(6);
    }
  */
}

/*
  UTIILITIES FUNCTIONS
*/

void lcdPrintBlank(byte number) {
  for (byte i = 0; i < number; i++) {
    lcd.print(" ");
  }
}

void setupRotary() {
  pinMode(ROT_A, INPUT_PULLUP);
  pinMode(ROT_B, INPUT_PULLUP);
  pinMode(ROT_PUSH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ROT_A), eventRotaryA, FALLING);
  attachInterrupt(digitalPinToInterrupt(ROT_PUSH), eventRotaryPressed, CHANGE);
}

boolean accelerationMode = false;
int lastIncrement = 0;

void eventRotaryA() {
  int increment = - (digitalRead(ROT_B) * 2 - 1);
  long current = millis();
  long diff = current - lastRotaryEvent;
  if (lastIncrement != increment && diff < 100) return;
  lastIncrement = increment;
  lastRotaryEvent = current;
  if (diff < 5) return;
  if (diff < 30) {
    if (accelerationMode) {
      rotaryCounter -= (increment * 20);
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
    if (rotaryMayPress && ((millis() - lastRotaryEvent) > 200)) {
      rotaryPressed = true;
      rotaryMayPress = false;
      lastRotaryEvent = millis();
    }
  } else {
    rotaryMayPress = true;
  }
}

#endif



