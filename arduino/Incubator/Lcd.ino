#define THR_LCD 1

#ifdef THR_LCD

#include <LiquidCrystal.h>
#include "libino/RotaryEncoder/Rotary.cpp"

#define LCD_E      12
#define LCD_RS     A6
#define LCD_D4     8
#define LCD_D5     9
#define LCD_D6     10
#define LCD_D7     5
#define LCD_BL     13    // back light


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

// Rotary encoder is wired with the common to ground and the two
// outputs to pins 2 and 3.
Rotary rotary = Rotary(ROT_A, ROT_B);

NIL_WORKING_AREA(waThreadLcd, 250);
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
    if (noEventCounter < 32760) noEventCounter++;
  } else {
    noEventCounter = 0;
  }
  if (noEventCounter > 250 && getParameter(PARAM_STATUS) == 0) {
    if (currentMenu - currentMenu % 10 != 20) currentMenu = 20;
    captureCounter = false;
  }
  boolean doAction = rotaryPressed;
  rotaryPressed = false;
  int counter = rotaryCounter;
  rotaryCounter = 0;

  switch (currentMenu - currentMenu % 10 ) {
    case 0:
      lcdMenuHome(counter, doAction);
      break;
    case 10:
      lcdMenuSettings(counter, doAction);
      break;
    case 20:
      lcdStatus(counter, doAction);
      break;
    case 40:
      lcdUtilities(counter, doAction);
      break;
  }
}

void lcdStatus(int counter, boolean doAction) {
  if (doAction) setParameter(PARAM_MENU, 0);
  updateCurrentMenu(counter, 2);
  if (noEventCounter < 2) lcd.clear();
  byte menu = getParameter(PARAM_MENU) % 10;
  switch (menu) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("T1:");
      lcd.print((double)getParameter(PARAM_TEMP_EXT_1)/100);
      lcd.print(" ");
      lcd.setCursor(8, 0);
      lcd.print("T2:");
      lcd.print((double)getParameter(PARAM_TEMP_EXT_2)/100);
      lcd.print(" ");
      lcdPrintBlank(2);
      lcd.setCursor(0, 1);
      lcd.print("Target:");
      lcd.print((double)getParameter(PARAM_TEMP_TARGET)/100);
      lcdPrintBlank(2);
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("PCB:");
      lcd.print((double)getParameter(PARAM_TEMP_PCB)/100);
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print("PID:");
      lcd.print(getParameter(PARAM_HBRIDGE_PID));
      lcd.print("  ");
      break;
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
  byte lastMenu = 1;
  updateCurrentMenu(counter, lastMenu);

  for (byte line = 0; line < LCD_NB_ROWS; line++) {
    lcd.setCursor(0, line);
    if ( getParameter(PARAM_MENU) % 10 + line <= lastMenu) lcdNumberLine(line);

    switch (getParameter(PARAM_MENU) % 10 + line) {

      case 0:
        lcd.print(F("Reboot"));
        if (doAction) {
          reboot();
        }
        break;
      case 1:
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

  byte lastMenu = 8;
  if (! captureCounter) updateCurrentMenu(counter, lastMenu);

  byte currentParameter = 0;
  int8_t currentParameterBit = -1;
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
      lcd.print(F("Rotary inverse"));
      currentParameter = PARAM_FLAGS;
      currentParameterBit = PARAM_FLAG_INVERT_ROTARY;
      minValue = 0;
      maxValue = 1;
      break;
    case 8:
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
    if (currentParameterBit == -1) {
      int newValue = getParameter(currentParameter) + counter;
      setParameter(currentParameter, max(min(maxValue, newValue), minValue));
    } else { // flag kind so either true or false
      if (counter > 0) {
        setParameterBit(currentParameter, currentParameterBit);
      } else if (counter < 0) {
        clearParameterBit(currentParameter, currentParameterBit);
      }
    }
  }

  lcd.setCursor(0, 1);
  if (millis() % 1000 < 500 && captureCounter) {
    lcd.print((char)255);
  } else {
    lcd.print(" ");
  }
  switch (getParameter(PARAM_MENU) % 10) {
    default:
      if (currentParameterBit == -1) {
        if (currentFactor == 1) {
          lcd.print((getParameter(currentParameter)));
        } else {
          lcd.print(((float)getParameter(currentParameter))*currentFactor);
        }
      } else {// it is a flag so we need to display true or false
        if (getParameterBit(currentParameter, currentParameterBit)) {
          lcd.print("true");
        } else {
          lcd.print("false");
        }
      }
      lcd.print(" ");
      lcd.print(currentUnit);
  }
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
  attachInterrupt(digitalPinToInterrupt(ROT_A), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_B), rotate, CHANGE);
  pinMode(ROT_PUSH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ROT_PUSH), eventRotaryPressed, CHANGE);
}

boolean accelerationMode = false;
int lastIncrement = 0;


boolean rotaryMayPress = true; // be sure to go through release. Seems to allow some deboucing


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


  if (getParameterBit(PARAM_FLAGS, PARAM_FLAG_INVERT_ROTARY) == 1) {
    increment *= -1;
  }

  if (accelerationMode > 4) {
    rotaryCounter += (increment * accelerationMode);
  } else {
    if (accelerationMode == 0) {
      rotaryCounter += increment;
    }
  }
}


void eventRotaryPressed() {
  cli();
  byte state = digitalRead(ROT_PUSH);
  long unsigned eventMillis = millis();
  if (state == 0) {
    if (rotaryMayPress && ((eventMillis - lastRotaryEvent) > 200)) {
      rotaryPressed = true;
      rotaryMayPress = false;
      lastRotaryEvent = eventMillis;
    }
  } else {
    rotaryMayPress = true;
    if ((eventMillis - lastRotaryEvent) > 5000) {
      reboot();
    }
  }
  sei();
}




#endif
