#include "GeniePrefs.h"

GeniePrefs::GeniePrefs() {
  openDuration = OPEN_DURATION_DEFAULT;
  keySounds = KEY_SOUND_DEFAULT;
  bootSound = BOOT_SOUND_DEFAULT;
  otherSounds = OTHER_SOUND_DEFAULT;
}

short GeniePrefs::readShort(int address, short vDefault) {
  short v = EEPROM.read(address);
  if ( v == DEFAULT_SHORT )
    v = vDefault;
  return v;
}

void GeniePrefs::writeShort(int address, short value) {
//#if DBG
//  Serial.print("GeniePrefs::writeShort: ");
//  Serial.print(address);
//  Serial.print(" = ");
//  Serial.println(value);
//#endif
  EEPROM.write(address, value);
}

int GeniePrefs::readInt(int address, int vDefault) {
  int v;
  EEPROM_readAnything(address, v);
  if ( v == DEFAULT_INT )
    v = vDefault;
  return v;
}

void GeniePrefs::writeInt(int address, int value) {
//#if DBG
//  Serial.print("GeniePrefs::writeInt: ");
//  Serial.print(address);
//  Serial.print(" = ");
//  Serial.println(value);
//#endif
  EEPROM_writeAnything(address, value);
}

long GeniePrefs::readLong(int address, long vDefault) {
  long v;
  EEPROM_readAnything(address, v);
  if ( v == DEFAULT_LONG )
    v = vDefault;
//#if DBG
//  Serial.print("GeniePrefs::readLong: ");
//  Serial.print(address);
//  Serial.print(" = ");
//  Serial.println(v);
//#endif
  return v;
}

void GeniePrefs::writeLong(int address, long value) {
//#if DBG
//  Serial.print("GeniePrefs::writeLong: ");
//  Serial.print(address);
//  Serial.print(" = ");
//  Serial.println(value);
//#endif
  EEPROM_writeAnything(address, value);
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
  openDuration = readInt(CFG_OPEN_DURATION, OPEN_DURATION_DEFAULT);
  keySounds = readBoolean(CFG_KEY_SOUND, KEY_SOUND_DEFAULT);
  bootSound = readBoolean(CFG_BOOT_SOUND, BOOT_SOUND_DEFAULT);
  otherSounds = readBoolean(CFG_OTHER_SOUND, OTHER_SOUND_DEFAULT);
  lock1 = readLong(CFG_LOCK1, LOCK1_DEFAULT);
  unlock1 = readLong(CFG_UNLOCK1, UNLOCK1_DEFAULT);
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
  writeInt(CFG_OPEN_DURATION, openDuration);
  writeBoolean(CFG_KEY_SOUND, keySounds);
  writeBoolean(CFG_BOOT_SOUND, bootSound);
  writeBoolean(CFG_OTHER_SOUND, otherSounds);
  writeLong(CFG_LOCK1, lock1);
  writeLong(CFG_UNLOCK1, unlock1);
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
  lock1 = LOCK1_DEFAULT;
  unlock1 = UNLOCK1_DEFAULT;
  save();
//#if DBG
//  Serial.println("GeniePrefs::resetToDefaults complete");
//#endif
}

void GeniePrefs::print() {
//#if DBG
//  Serial.print("openDuration = "); Serial.println(openDuration);
//  Serial.print("keySounds = "); Serial.println(keySounds);
//  Serial.print("bootSound = "); Serial.println(bootSound);
//  Serial.print("otherSounds = "); Serial.println(otherSounds);
//#endif
}

void GeniePrefs::flush() {
  for (int i=0; i < 512; i++) {
    EEPROM.write(i, DEFAULT_SHORT);
  }
}
