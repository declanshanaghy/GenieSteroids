#ifndef GENIEPREFS_H
#define GENIEPREFS_H

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include <EEPROM.h>

#define CFG_WRITTEN 0
#define CFG_MAG_DELAY 1

class GeniePrefs {
public:
  void load();
  void resetToDefault();
  void save();
  void printPrefs();

  short magDelayMins; //Time in minutes that the mag switch must 
                      //indicate the door is open before it will 
                      //be auto closed (0 = off)
};

#endif //GENIEPREFS_H

