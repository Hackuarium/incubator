#include <OneWire.h>

//#define DEBUG_ONEWIRE 1

byte oneWireAddress[8];

#ifdef TEMPERATURE_EXT_1
OneWire oneWire1(TEMPERATURE_EXT_1);
#endif

#ifdef TEMPERATURE_EXT_2
OneWire oneWire2(TEMPERATURE_EXT_2);
#endif

#ifdef TEMPERATURE_PCB
OneWire oneWire3(TEMPERATURE_PCB);
#endif


void getTemperature(OneWire &ow, int parameter, byte errorFlag);
void oneWireInfoSS(OneWire &ow, Print* output) ;

NIL_WORKING_AREA(waThreadTemperature, 300);
NIL_THREAD(ThreadTemperature, arg) {
  nilThdSleepMilliseconds(1000);
#ifdef DEBUG_ONEWIRE
  nilThdSleepMilliseconds(2000);
#endif

  while (true) {
#ifdef TEMPERATURE_EXT_1
    getTemperature(oneWire1, PARAM_TEMP_EXT_1, FLAG_TEMP_EXT_1_PROBE_ERROR);
#endif

#ifdef TEMPERATURE_EXT_2
    getTemperature(oneWire2, PARAM_TEMP_EXT_2, FLAG_TEMP_EXT_2_PROBE_ERROR);
#endif

#ifdef TEMPERATURE_PCB
    getTemperature(oneWire3, PARAM_TEMP_PCB, FLAG_TEMP_PCB_PROBE_ERROR);
#endif
    nilThdSleepMilliseconds(200);
  }
}


void getTemperature(OneWire &ow, int parameter, byte errorFlag) {
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;

  if ( !ow.search(addr)) {
#ifdef DEBUG_ONEWIRE
    Serial.println(F("No more addr."));
    Serial.println();
#endif
    ow.reset_search();
    nilThdSleepMilliseconds(200);
    if (!ow.search(addr)) {
      saveAndLogError(true, errorFlag);
#ifdef DEBUG_ONEWIRE
      Serial.println(F("Error getting TP"));
#endif
      setParameter(parameter, ERROR_VALUE);
      return;
    } else {
      saveAndLogError(false, errorFlag);
    }
  } else {
    saveAndLogError(false, errorFlag);
  }

#ifdef DEBUG_ONEWIRE
  Serial.print(F("ROM ="));
  for ( byte i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
#endif

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println(F("CRC invalid!"));
    return;
  }

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
#ifdef DEBUG_ONEWIRE
      Serial.println(F("  Chip = DS18S20"));  // or old DS1820
#endif
      type_s = 1;
      break;
    case 0x28:
#ifdef DEBUG_ONEWIRE
      Serial.println(F("  Chip = DS18B20"));
#endif
      type_s = 0;
      break;
    case 0x22:
#ifdef DEBUG_ONEWIRE
      Serial.println(F("  Chip = DS1822"));
#endif
      type_s = 0;
      break;
    default:
#ifdef DEBUG_ONEWIRE
      Serial.println(F("Device is not a DS18x20"));
#endif
      return;
  }

  protectThread();
  ow.reset();
  ow.select(addr);
  ow.write(0x44, 1);        // start conversion, with parasite power on at the end
  unprotectThread();

  nilThdSleepMilliseconds(800);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  protectThread();
  present = ow.reset();
  ow.select(addr);
  ow.write(0xBE);         // Read Scratchpad
  unprotectThread();

#ifdef DEBUG_ONEWIRE
  Serial.print(F("  Data = "));
  Serial.print(present, HEX);
  Serial.print(" ");
#endif
  for (byte i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ow.read();
#ifdef DEBUG_ONEWIRE
    Serial.print(data[i], HEX);
    Serial.print(" ");
#endif
  }
#ifdef DEBUG_ONEWIRE
  Serial.print(F(" CRC="));
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();
#endif

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
#ifdef DEBUG_ONEWIRE
  Serial.print(F("  T = "));
  Serial.print(celsius);
  Serial.println(F("°C"));
#endif
  setParameter(parameter, (int)(celsius * 100));
  ow.reset_search();
  nilThdSleepMilliseconds(1000);
}



//bus info function
void oneWireInfo(Print* output) { // TODO
#ifdef TEMP_LIQ
  output->println(F("One wire liquid device list"));
  oneWireInfoSS(oneWire1, output);
#endif
#ifdef TEMP_PCB
  output->println(F("One wire PCB device list"));
  oneWireInfoSS(oneWire2, output);
#endif
}

void oneWireInfoSS(OneWire &ow, Print* output) { // TODO
  protectThread();
  ow.reset_search();
  while (ow.search(oneWireAddress)) {
    for (byte i = 0; i < 8; i++) {
      output->print(' ');
      output->print(oneWireAddress[i], HEX);
    }
    output->println("");
    nilThdSleepMilliseconds(250);
  }
  unprotectThread();
}

