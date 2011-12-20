#ifndef GENIRPREFS_H
#define GENIRPREFS_H

#include <Arduino.h>
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

#endif //GENIRPREFS_H

