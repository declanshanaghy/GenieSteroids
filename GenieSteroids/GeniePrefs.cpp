#include "GeniePrefs.h"

void GeniePrefs::load() {
  Serial.println("GeniePrefs::load...");
  printPrefs();
  short v = EEPROM.read(CFG_WRITTEN);
  if ( v != 255 ) {
    magDelayMins = EEPROM.read(CFG_MAG_DELAY);
  }
  else {
    resetToDefault();
  }
  Serial.println("GeniePrefs::load complete");
}

void GeniePrefs::save() {
  Serial.println("GeniePrefs::save...");
  printPrefs();
  EEPROM.write(CFG_MAG_DELAY, magDelayMins);
  EEPROM.write(CFG_WRITTEN, 1);
  Serial.println("GeniePrefs::save complete");
}

void GeniePrefs::resetToDefault() {
  Serial.println("GeniePrefs::resetToDefaults...");
  EEPROM.write(CFG_WRITTEN, 255);
  magDelayMins = 10;
  save();
  Serial.println("GeniePrefs::resetToDefaults complete");
}

void GeniePrefs::printPrefs() {
  Serial.print("magDelayMins="); Serial.println(magDelayMins);
}
