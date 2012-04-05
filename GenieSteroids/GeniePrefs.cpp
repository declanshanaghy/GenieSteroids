#include "GeniePrefs.h"

GeniePrefs::GeniePrefs() {
  openDuration = OPEN_DURATION_DEFAULT;
  keySounds = KEY_SOUNDS_DEFAULT;
  bootSound = BOOT_SOUND_DEFAULT;
  otherSounds = OTHER_SOUNDS_DEFAULT;
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
  Serial.println("GeniePrefs::load...");
  print();
  openDuration = readShort(CFG_OPEN_DURATION, OPEN_DURATION_DEFAULT);
  keySounds = readBoolean(CFG_KEY_SOUNDS, KEY_SOUNDS_DEFAULT);
  bootSound = readBoolean(CFG_BOOT_SOUND, BOOT_SOUND_DEFAULT);
  otherSounds = readBoolean(CFG_OTHER_SOUNDS, OTHER_SOUNDS_DEFAULT);
  Serial.println("GeniePrefs::load complete");
}

void GeniePrefs::save() {
  Serial.println("GeniePrefs::save...");
  print();
  writeShort(CFG_OPEN_DURATION, openDuration);
  writeBoolean(CFG_KEY_SOUNDS, keySounds);
  writeBoolean(CFG_BOOT_SOUND, bootSound);
  writeBoolean(CFG_OTHER_SOUNDS, otherSounds);
  Serial.println("GeniePrefs::save complete");
}

void GeniePrefs::resetToDefault() {
  Serial.println("GeniePrefs::resetToDefaults...");
  openDuration = OPEN_DURATION_DEFAULT;
  keySounds = KEY_SOUNDS_DEFAULT;
  bootSound = KEY_SOUNDS_DEFAULT;
  otherSounds = KEY_SOUNDS_DEFAULT;
  save();
  Serial.println("GeniePrefs::resetToDefaults complete");
}

void GeniePrefs::print() {
  Serial.print("openDuration = "); Serial.println(openDuration);
  Serial.print("keySounds = "); Serial.println(keySounds);
}
