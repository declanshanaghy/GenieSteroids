#ifndef GENIEPREFS_H
#define GENIEPREFS_H

#include <Arduino.h>
#include <EEPROM.h>

//00-09 : Internally reserved 

//10-19 : Lock times
#define CFG_LOCK1  10
#define CFG_LOCK2  12

//20-XX : Settings
#define CFG_OPEN_DURATION  20
#define CFG_BACKLIGHT      30
#define CFG_BOOT_SOUND     40
#define CFG_KEY_SOUNDS     50
#define CFG_BACKLIGHT      60
#define CFG_OTHER_SOUNDS   70

#define OPEN_DURATION_DEFAULT  10
#define KEY_SOUNDS_DEFAULT     true
#define BOOT_SOUND_DEFAULT     true
#define OTHER_SOUNDS_DEFAULT   true

class GeniePrefs {
public:
  GeniePrefs();
  void load();
  void resetToDefault();
  void save();
  void print();

  short readShort(int address, short vDefault);
  void writeShort(int address, short value);
  boolean readBoolean(int address, boolean vDefault);
  void writeBoolean(int address, boolean value);

  // Time in minutes that the door can stay open
  // before being closed automatically
  short openDuration; 

  // Whether to play key sounds when keys are pressed
  boolean keySounds;
  
  // Whether to play key sounds when the device boots
  boolean bootSound;
  
  // Whether to play sounds upon confirmation & cancel
  boolean otherSounds;
};

#endif //GENIEPREFS_H

