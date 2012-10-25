
#include "LockManager.h"

LockManager::LockManager(event_cb cb) : cb(cb) { 
  events[0].lock = true;
  events[0].tCpu = 0;
  
  events[1].lock = false;
  events[1].tCpu = 0;
}

void LockManager::loop() {
  unsigned long tNow = millis();
  if ( tLastUpdate == 0 || tNow > tLastUpdate + LOOP_UPDATE_INTERVAL ) {
    procLoopLockMgr(tNow);
    tLastUpdate = tNow;
  }  
}

void LockManager::procLoopLockMgr(unsigned long tNow) {
  // Check if midnight just passed and the event times should be updated
  if ( tMidnight == 0 ) {
    recalcMidnight();
  }
  
  if ( tNow > tMidnight ) {
    tMidnight = 0;
    recalcEvents();
  }

  // Check if the door should be locked now
  if ( events[0].tCpu != 0 && tNow > events[0].tCpu ) {
    event_cb(MSG_LOCK_NOW);
    events[0].tCpu = 0;
  }
  // Check if the door should be unlocked now
  if ( events[1].tCpu != 0 && tNow > events[1].tCpu ) {
    event_cb(MSG_UNLOCK_NOW);
    events[1].tCpu = 0;
  }
}

void LockManager::recalcMidnight() {
  DateTime now = chronodot.now();
  DateTime midnight = chronodot.now();
  midnight.setHour(0);
  midnight.setMinute(0);
  midnight.setSecond(0);
  
  // Calc time since midnight today
  unsigned long secsSinceMidnight = now.unixtime() - midnight.unixtime();
  
  // Calc time until next midnight
  unsigned long secsUntilNextMidnight = 86400L - secsSinceMidnight;

  // Record midnight in CPU time
  tMidnight = millis() + (1000L * secsUntilNextMidnight);
}

void LockManager::recalcEvents() { 
  setEvents(events[0].dt, events[1].dt);
}

void LockManager::setEvents(DateTime lock, DateTime unlock) { 
  events[0].dt = lock; 
  events[0].tCpu = dateTimeToCpuTime(lock); 
  
  events[1].dt = unlock;
  events[1].tCpu = dateTimeToCpuTime(unlock); 
}

unsigned long LockManager::dateTimeToCpuTime(DateTime dt) {
  DateTime now = chronodot.now();
  
  // We only care about time, not date
  dt.setYear(now.year());
  dt.setMonth(now.month());
  dt.setDay(now.day());
  
  long diff = dt.unixtime() - now.unixtime();
  if ( diff > 0 )
    return diff + millis();   
  else
    return 0;
}

