#ifndef GENIEPREFS_H
#define GENIEPREFS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "EEPromAnything.h"

#define DBG 0

//00-09 : Internally reserved 
#define CFG_UNUSED         0

//10-19 : Lock times
#define CFG_LOCK1          10
#define CFG_LOCK2          12

//20-XX : Settings
#define CFG_OPEN_DURATION  20
#define CFG_BACKLIGHT      30
#define CFG_BOOT_SOUND     40
#define CFG_KEY_SOUND      50
#define CFG_BACKLIGHT      60
#define CFG_OTHER_SOUND    70
#define CFG_LOCK1          80
#define CFG_UNLOCK1        84

#define OPEN_DURATION_DEFAULT  10
#define KEY_SOUND_DEFAULT      true
#define BOOT_SOUND_DEFAULT     true
#define OTHER_SOUND_DEFAULT    true
#define LOCK1_DEFAULT          0
#define UNLOCK1_DEFAULT        0

#define DEFAULT_SHORT  0xFF
#define DEFAULT_INT    0xFFFF
#define DEFAULT_LONG   0xFFFFFFFF

class GeniePrefs {
public:
  GeniePrefs();
  void load();
  void resetToDefault();
  void flush();
  void save();
  void print();

  short readShort(int address, short vDefault);
  void writeShort(int address, short value);
  
  int readInt(int address, int vDefault);
  void writeInt(int address, int value);

  long readLong(int address, long vDefault);
  void writeLong(int address, long value);

  boolean readBoolean(int address, boolean vDefault);
  void writeBoolean(int address, boolean value);

  // Time in minutes that the door can stay open
  // before being closed automatically
  int openDuration; 

  // Whether to play key sounds when keys are pressed
  boolean keySounds;
  
  // Whether to play key sounds when the device boots
  boolean bootSound;
  
  // Whether to play sounds upon confirmation & cancel
  boolean otherSounds;
  
  // Times to lock and unlock the door
  long lock1;
  long unlock1;
};

#endif //GENIEPREFS_H

