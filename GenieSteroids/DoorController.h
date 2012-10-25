#ifndef DOORCONTROLLER_H
#define DOORCONTROLLER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Bounce.h>
#include "GeniePrefs.h"

#define STATE_DOOR_CLOSED 0
#define STATE_DOOR_OPEN 1
#define STATE_DOOR_CLOSING 2

#define DOOR_ALARM_SECONDS 10
#define RELAY_DELAY 100

#define MSG_DOOR_OPEN 0              // The door state changed to open (void)
#define MSG_CLOSE_DOOR_NOW 1         // Door is being activated to be closed now
#define MSG_CLOSE_DOOR_COUNTDOWN 2   // Door is open, countdown is in progress (unsigned long millisUntilClose)
#define MSG_DOOR_CLOSING 3           // Door is closing (unsigned long millisUntilError)
#define MSG_DOOR_CLOSED 4            // Door is closed (void)
#define MSG_DOOR_CLOSE_ERROR 5       // Error closing door

#define LOOP_UPDATE_INTERVAL 1000

typedef void (*event_cb)(short msg, unsigned long countdown);

class DoorController {
public:
  DoorController(event_cb cb, GeniePrefs *prefs, short doorSensor, short doorRelay, int doorRelayDelay) 
    : cb(cb), prefs(prefs), door(doorSensor, 100), doorRelay(doorRelay), doorRelayDelay(doorRelayDelay),
      doorState(STATE_DOOR_CLOSED), tCloseDoorAt(0) {
    pinMode(doorSensor, INPUT); 
    pinMode(doorRelay, OUTPUT); 
  };
  void loop();
  boolean isDoorOpen();
  void activateDoorRelay();
  
private:
  void procLoopDoor(unsigned long tNow);
    
  event_cb cb;
  GeniePrefs *prefs;
  Bounce door;
  short doorState;
  short doorSensor;
  short doorRelay;
  int doorRelayDelay;
  unsigned long tCloseDoorAt;
  unsigned long tLastUpdate;
};

#endif DOORCONTROLLER_H

