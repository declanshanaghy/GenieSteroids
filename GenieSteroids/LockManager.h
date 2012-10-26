#ifndef LOCKMANAGER_H
#define LOCKMANAGER_H

#include <Arduino.h>
#include <Chronodot.h>

#define DBG 1

#define MSG_LOCK_NOW 0           // Notification to lock the door now
#define MSG_UNLOCK_NOW 1         // Notification to unlock the door now

#define LOOP_UPDATE_INTERVAL 1000

typedef void (*event_cb)(short msg, unsigned long countdown);

typedef struct LockEvent_t {
  boolean lock;              // True if the door should be locked, false to unlock
  DateTime dt;               // The DateTime the user requests the lock action to happen
  unsigned long tCpu;    // Next CPU time at which the event should occur
} LockEvent;

class LockManager {
public:
  LockManager(event_cb cb);
  void setEvents(DateTime lock, DateTime unlock);
  void loop(unsigned long tNow);
  void reset() { tLastUpdate = 0; };
  
private:
  unsigned long dateTimeToCpuTime(DateTime dt);
  void recalcEvents();
  void recalcMidnight();
  void procLoopLockMgr(unsigned long tNow);
  
  Chronodot chronodot;
  LockEvent events[2];
  event_cb cb;
  unsigned long tLastUpdate;
  unsigned long tMidnight;
};

#endif LOCKMANAGER_H
