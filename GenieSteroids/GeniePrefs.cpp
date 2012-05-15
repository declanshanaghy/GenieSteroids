#include "GeniePrefs.h"

GeniePrefs::GeniePrefs() {
  openDuration = OPEN_DURATION_DEFAULT;
  keySounds = KEY_SOUND_DEFAULT;
  bootSound = BOOT_SOUND_DEFAULT;
  otherSounds = OTHER_SOUND_DEFAULT;
}

short GeniePrefs::readShort(int address, short vDefault) {
  short val = EEPROM.read(address);
  if ( val == 255 )
    return vDefault;
  else
    return val;
}

void GeniePrefs::writeShort(int address, short value) {
  EEPROM.write(address, value);
}

boolean GeniePrefs::readBoolean(int address, boolean vDefault) {
  short val = readShort(address, 255);
  if (val == 255)
    return vDefault;
  else
     return val == 1;
}

void GeniePrefs::writeBoolean(int address, boolean value) {
  writeShort(address, value);
}

void GeniePrefs::load() {
//#if DBG
//  Serial.println("GeniePrefs::load...");
//#endif
  openDuration = readShort(CFG_OPEN_DURATION, OPEN_DURATION_DEFAULT);
  keySounds = readBoolean(CFG_KEY_SOUND, KEY_SOUND_DEFAULT);
  bootSound = readBoolean(CFG_BOOT_SOUND, BOOT_SOUND_DEFAULT);
  otherSounds = readBoolean(CFG_OTHER_SOUND, OTHER_SOUND_DEFAULT);
//#if DBG
//  Serial.println("GeniePrefs::load complete");
//#endif
  print();
}

void GeniePrefs::save() {
//#if DBG
//  Serial.println("GeniePrefs::save...");
//#endif
  print();
  writeShort(CFG_OPEN_DURATION, openDuration);
  writeBoolean(CFG_KEY_SOUND, keySounds);
  writeBoolean(CFG_BOOT_SOUND, bootSound);
  writeBoolean(CFG_OTHER_SOUND, otherSounds);
//#if DBG
//  Serial.println("GeniePrefs::save complete");
//#endif
}

void GeniePrefs::resetToDefault() {
//#if DBG
//  Serial.println("GeniePrefs::resetToDefaults...");
//#endif
  openDuration = OPEN_DURATION_DEFAULT;
  keySounds = KEY_SOUND_DEFAULT;
  bootSound = BOOT_SOUND_DEFAULT;
  otherSounds = OTHER_SOUND_DEFAULT;
  save();
//#if DBG
//  Serial.println("GeniePrefs::resetToDefaults complete");
//#endif
}

void GeniePrefs::print() {
//#if DBG
  Serial.print("openDuration = "); Serial.println(openDuration);
  Serial.print("keySounds = "); Serial.println(keySounds);
  Serial.print("bootSound = "); Serial.println(bootSound);
  Serial.print("otherSounds = "); Serial.println(otherSounds);
//#endif
}
