#define THR_LCD 1

#define LANGUAGE en

#ifdef THR_LCD

#if LANGUAGE == en
#define TEXT_ABSORBANCE "Absorb."
#define TEXT_WAITING_BLANK "Waiting blank"
#define TEXT_WAITING_EXP "Waiting exp."
#define TEXT_ACQUIRING "Acquiring"
#define TEXT_BLANK "Blank"
#define TEXT_SAMPLE "Sample"
#define TEXT_KINETIC "Kinetic"
#define TEXT_STOP "Stop acquis."
#define TEXT_ACQUIRE "Acquire"
#define TEXT_ACQ_KINETIC "Acq. kinetic"
#define TEXT_RESULTS "Results"
#define TEXT_SETTINGS "Settings"
#define TEXT_STATUS "Status"
#define TEXT_UTILITIES "Utilities"
#define TEXT_BACKLIGHT "Backlight"
#define TEXT_TEST_LEDS "Test LED"
#define TEXT_RESET "Reset"
#define TEXT_MAIN_MENU "Main menu"
#define TEXT_RED "Red"
#define TEXT_GREEN "Green"
#define TEXT_BLUE "Blue"
#define TEXT_UV1 "UV 1"
#define TEXT_UV2 "UV 2"
#define TEXT_BEFORE_DELAY "Before delay"
#define TEXT_FIRST_DELAY "First delay"
#define TEXT_INTER_DELAY "Inter exp. delay"
#define TEXT_NUMBER_EXP "Number exp."
#define TEXT_RESULT_COLOR "Result color"
#endif

#if LANGUAGE == es
#define TEXT_ABSORBANCE "Absorb."
#define TEXT_WAITING_BLANK "Esperando blanco"
#define TEXT_WAITING_EXP "Esperando exp."
#define TEXT_ACQUIRING "Adquiriendo"
#define TEXT_BLANK "Blanco"
#define TEXT_SAMPLE "Muestra"
#define TEXT_KINETIC "Cinetica"
#define TEXT_STOP "Detener"
#define TEXT_ACQUIRE "Adquirir"
#define TEXT_ACQ_KINETIC "Adq. Cinetica"
#define TEXT_RESULTS "Resultados"
#define TEXT_SETTINGS "Ajustes"
#define TEXT_STATUS "Estado"
#define TEXT_UTILITIES "Utilidades"
#define TEXT_BACKLIGHT "Iluminar"
#define TEXT_TEST_LEDS "prueba de LED"
#define TEXT_RESET "Reiniciar"
#define TEXT_MAIN_MENU "Menu"
#define TEXT_RED "Rojo"
#define TEXT_GREEN "Verde"
#define TEXT_BLUE "Azul"
#define TEXT_UV1 "UV 1"
#define TEXT_UV2 "UV 2"
#define TEXT_BEFORE_DELAY "Tiempo inicio"
#define TEXT_FIRST_DELAY "Primera pausa"
#define TEXT_INTER_DELAY "Tiempo entre exp"
#define TEXT_NUMBER_EXP "Numero de exp"
#define TEXT_RESULT_COLOR "Color"
#endif

long data[MAX_EXPERIMENTS * 6]; // epoch R G B UV1 UV2

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
    case 30:
      lcdAcquisition(counter, doAction);
      break;
    case 40:
      lcdUtilities(counter, doAction);
      break;
    case 100:
      lcdResults(counter, doAction);
      break;
  }
}

void lcdResults(int counter, boolean doAction) {
  if (doAction) setParameter(PARAM_MENU, 0);
  if (noEventCounter < 2) lcd.clear();

  // calculate the last experiment based on epoch of each experiment
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
}

void lcdDefault(int counter, boolean doAction) {
  if (doAction) setParameter(PARAM_MENU, 0);
  updateCurrentMenu(counter, 6);
  if (noEventCounter < 2) lcd.clear();
  byte menu = getParameter(PARAM_MENU) % 10;
  if (menu < 5) {
    lcd.setCursor(0, 0);
    lcdPrintColor(menu);
    lcd.setCursor(0, 1);
    lcd.print(F(TEXT_ABSORBANCE));
    lcd.setCursor(8, 1);
    if (getParameter(menu + 5) == INT_MAX_VALUE || getParameter(menu) == INT_MAX_VALUE) {
      lcd.print(F("OVER"));
    } else {
      lcd.print(log10((double)getParameter(menu + 5) / (double)getParameter(menu)));
    }
    lcdPrintBlank(2);
  } else {
    lcd.setCursor(0, 0);
    epochToString(now(), &lcd);
    lcd.setCursor(6, 1);
    lcd.print("s:");
    lcd.print(millis() / 1000);
  }
}

void lcdDefaultExact(int counter, boolean doAction) {
  if (doAction) setParameter(PARAM_MENU, 0);
  updateCurrentMenu(counter, 2);
  if (noEventCounter < 2) lcd.clear();
  switch (getParameter(PARAM_MENU) % 10) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("R:");
      lcd.print(getParameter(PARAM_BLANK_R) - getParameter(PARAM_R));
      lcdPrintBlank(2);
      lcd.setCursor(8, 0);
      lcd.print("G:");
      lcd.print(getParameter(PARAM_BLANK_G) - getParameter(PARAM_G));
      lcdPrintBlank(2);
      lcd.setCursor(0, 1);
      lcd.print("B:");
      lcd.print(getParameter(PARAM_BLANK_B) - getParameter(PARAM_B));
      lcdPrintBlank(2);
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("UV 1: ");
      lcd.print(getParameter(PARAM_BLANK_UV1) - getParameter(PARAM_UV1));
      lcdPrintBlank(2);
      lcd.setCursor(0, 1);
      lcd.print("UV 2: ");
      lcd.print(getParameter(PARAM_BLANK_UV2) - getParameter(PARAM_UV2));
      break;
    case 2:
      lcd.setCursor(0, 0);
      epochToString(now(), &lcd);
      lcd.setCursor(6, 1);
      lcd.print("s:");
      lcd.print(millis() / 1000);
      break;
  }
}

void lcdAcquisition(int counter, boolean doAction) {
  if (doAction) setParameter(PARAM_MENU, 0);
  if (noEventCounter < 2) lcd.clear();
  switch (getParameter(PARAM_MENU) % 10) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print(F(TEXT_WAITING_BLANK));
      lcdWait();
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print(F(TEXT_WAITING_EXP));
      lcd.print(getParameter(PARAM_NEXT_EXP));
      lcdWait();
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(F(TEXT_ACQUIRING));
      lcd.setCursor(0, 1);
      if (getParameter(PARAM_NEXT_EXP) == 0) {
        lcd.print(F(TEXT_BLANK));
      } else if (getParameter(PARAM_NEXT_EXP) == 1) {
        lcd.print(F(TEXT_SAMPLE));
      } else if (getParameter(PARAM_NEXT_EXP) > 1) {
        lcd.print(F(TEXT_KINETIC));
        lcd.print(getParameter(PARAM_NEXT_EXP));
      }
      break;
  }
}

void lcdWait() {
  lcd.setCursor(0, 1);
  lcd.print(getParameter(PARAM_WAIT));
  lcd.print(" s ");
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
        if (getParameter(PARAM_NEXT_EXP) >= 0) {
          lcd.print(F(TEXT_STOP));
          if (doAction) {
            setParameter(PARAM_NEXT_EXP, -1);
          }
        } else {
          lcd.print(F(TEXT_ACQUIRE));
          if (doAction) {
            setParameter(PARAM_STATUS, STATUS_ONE_SPECTRUM);
            setParameter(PARAM_NEXT_EXP, 0);
          }
        }
        break;
      case 1:
        lcd.print(F(TEXT_ACQ_KINETIC));
        if (doAction) {
          setParameter(PARAM_STATUS, STATUS_KINETIC);
          setParameter(PARAM_NEXT_EXP, 0);
        }
        break;
      case 2:
        lcd.print(F(TEXT_RESULTS));
        if (doAction) {
          setParameter(PARAM_MENU, 100);
        }
        break;
      case 3:
        lcd.print(F(TEXT_SETTINGS));
        if (doAction) {
          setParameter(PARAM_MENU, 10);
        }
        break;
      case 4:
        lcd.print(F(TEXT_STATUS));
        if (doAction) {
          setParameter(PARAM_MENU, 20);
        }
        break;
      case 5:
        lcd.print(F(TEXT_UTILITIES));
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
  byte lastMenu = 3;
  updateCurrentMenu(counter, lastMenu);

  for (byte line = 0; line < LCD_NB_ROWS; line++) {
    lcd.setCursor(0, line);
    if ( getParameter(PARAM_MENU) % 10 + line <= lastMenu) lcdNumberLine(line);

    switch (getParameter(PARAM_MENU) % 10 + line) {
      case 0:
        lcd.print(F(TEXT_BACKLIGHT));
        if (doAction) {
          digitalWrite(LCD_BL, !digitalRead(LCD_BL));
        }
        break;
      case 1:
        lcd.print(F(TEXT_TEST_LEDS));
        if (doAction) {
          testRGB();
        }
        break;
      case 2:
        lcd.print(F(TEXT_RESET));
        if (doAction) {
          resetParameters();
          setParameter(PARAM_MENU, 20);
        }
        break;
      case 3:
        lcd.print(F(TEXT_MAIN_MENU));
        if (doAction) {
          setParameter(PARAM_MENU, 1);
        }
        return;
    }
    doAction = false;
  }
}

void lcdPrintColor(byte color) {
  switch (color) {
    case 0:
      lcd.print(F(TEXT_RED));
      break;
    case 1:
      lcd.print(F(TEXT_GREEN));
      break;
    case 2:
      lcd.print(TEXT_BLUE);
      break;
    case 3:
      lcd.print(TEXT_UV1);
      break;
    case 4:
      lcd.print(TEXT_UV2);
      break;
  }
}

void lcdMenuSettings(int counter, boolean doAction) {

  byte lastMenu = 5;
  if (! captureCounter) updateCurrentMenu(counter, lastMenu);

  byte currentParameter = 0;
  float currentFactor = 1;
  char currentUnit[5] = "\0";
  int maxValue = 32767;
  int minValue = -32768;

  lcd.clear();

  switch (getParameter(PARAM_MENU) % 10) {
    case 0:
      lcd.print(F(TEXT_BEFORE_DELAY));
      currentParameter = PARAM_BEFORE_DELAY;
      minValue = 0;
      strcpy(currentUnit, "s\0");
      break;
    case 1:
      lcd.print(F(TEXT_FIRST_DELAY));
      currentParameter = PARAM_FIRST_DELAY;
      minValue = 0;
      strcpy(currentUnit, "s\0");
      break;
    case 2:
      lcd.print(F(TEXT_INTER_DELAY));
      currentParameter = PARAM_INTER_DELAY;
      minValue = 0;
      strcpy(currentUnit, "s\0");
      break;
    case 3:
      lcd.print(F(TEXT_NUMBER_EXP));
      currentParameter = PARAM_NUMPER_EXP;
      minValue = 1;
      maxValue = MAX_EXPERIMENTS;
      break;
    case 4:
      lcd.print(F(TEXT_RESULT_COLOR));
      currentParameter = PARAM_COLOR;
      minValue = 1;
      maxValue = sizeof(LEDS);
      break;
    case 5:
      lcd.print(F(TEXT_MAIN_MENU));
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
    case 4: //
      lcdPrintColor(getParameter(currentParameter) - 1);
      break;
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
  int increment = digitalRead(ROT_B) * 2 - 1;
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



