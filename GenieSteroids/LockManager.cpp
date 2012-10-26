
#include "LockManager.h"

LockManager::LockManager(event_cb cb) : cb(cb), tLastUpdate(0), tMidnight(0) { 
  events[0].lock = true;
  events[0].tCpu = 0;
  
  events[1].lock = false;
  events[1].tCpu = 0;
}

void LockManager::loop(unsigned long tNow) {
//#if DBG
//  Serial.print("LockManager::loop: ");
//  Serial.println(tNow);
//#endif
  if ( tLastUpdate == 0 || tNow - tLastUpdate > LOOP_UPDATE_INTERVAL ) {
    procLoopLockMgr(tNow);
    tLastUpdate = tNow;
  }  
}

void LockManager::procLoopLockMgr(unsigned long tNow) {
  // Check if midnight just passed and the event times should be updated
//#if DBG
//  Serial.print("LockManager::procLoopLockMgr: ");
//  Serial.println(tNow);
//#endif

  if ( tMidnight == 0 ) {
    recalcMidnight();
  }
  
  if ( tNow > tMidnight ) {
    tMidnight = 0;
    recalcEvents();
  }

  // Check if the door should be locked now
  if ( events[0].tCpu != 0 && tNow > events[0].tCpu ) {
    cb(MSG_LOCK_NOW, 0);
    events[0].tCpu = 0;
//#if DBG
//  Serial.println("MSG_LOCK_NOW (tx)");
//#endif
  }
  // Check if the door should be unlocked now
  if ( events[1].tCpu != 0 && tNow > events[1].tCpu ) {
    cb(MSG_UNLOCK_NOW, 0);
    events[1].tCpu = 0;
//#if DBG
//  Serial.println("MSG_UNLOCK_NOW (tx)");
//#endif
  }
}

void LockManager::recalcMidnight() {
//#if DBG
//  Serial.print("recalcMidnight...");
//#endif
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
//#if DBG
//  Serial.print("tMidnight: ");
//  Serial.println(tMidnight);
//#endif
}

void LockManager::recalcEvents() {
//#if DBG
//  Serial.print("recalcEvents...");
//#endif
  setEvents(events[0].dt, events[1].dt);
}

void LockManager::setEvents(DateTime lock, DateTime unlock) { 
//#if DBG
//  Serial.println("setEvents...");
//#endif
  events[0].dt = lock; 
  events[0].tCpu = dateTimeToCpuTime(lock); 
  
  events[1].dt = unlock;
  events[1].tCpu = dateTimeToCpuTime(unlock);  
//#if DBG
//  Serial.print("now: ");
//  Serial.println(millis());
//  Serial.print("lock: ");
//  Serial.println(events[0].tCpu);
//#endif
}

unsigned long LockManager::dateTimeToCpuTime(DateTime dt) {
  DateTime now = chronodot.now();
  
//#if DBG
//  Serial.println("dateTimeToCpuTime...");
//  Serial.print("dt.hour: ");
//  Serial.println(dt.hour());
//  Serial.print("dt.minute: ");
//  Serial.println(dt.minute());
//  Serial.print("dt.second: ");
//  Serial.println(dt.second());
//#endif

  // We only care about time, not date
  dt.setYear(now.year());
  dt.setMonth(now.month());
  dt.setDay(now.day());
  
  long diff = dt.unixtime() - now.unixtime();
  unsigned long cpu = (1000L * diff) + millis();   
//#if DBG
//  Serial.print("diff: ");
//  Serial.println(diff);
//  Serial.print("cpu: ");
//  Serial.println(cpu);
//#endif
  if ( diff > 0 )
    return cpu;
  else
    return 0;
}

