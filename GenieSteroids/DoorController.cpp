#include "DoorController.h"

void DoorController::loop(unsigned long tNow) {
  if ( tLastUpdate == 0 || tNow - tLastUpdate > LOOP_UPDATE_INTERVAL ) {
    procLoopDoor(tNow);
    tLastUpdate = tNow;
  }
}

void DoorController::procLoopDoor(unsigned long tNow) {
  boolean doorOpenNow = isDoorOpen();
    
  if ( (doorState == STATE_DOOR_OPEN && !doorOpenNow) || 
       (doorState == STATE_DOOR_CLOSED && doorOpenNow) ||
       (doorState == STATE_DOOR_CLOSING && !doorOpenNow) ) {
    // Door state changed
    if ( doorOpenNow ) {
      // Start the timer for counting down to when the door will be closed
      tCloseDoorAt = tNow + 1000L * 60 * prefs->openDuration;
      doorState = STATE_DOOR_OPEN;
      cb(MSG_DOOR_OPEN, 0);
    }
    else {
      // Ensure the timer that closes the door is canceled
      tCloseDoorAt = 0;
      doorState = STATE_DOOR_CLOSED;
      cb(MSG_DOOR_CLOSED, 0);
    }
  }
  else if ( doorState == STATE_DOOR_OPEN ) {
    if ( tNow > tCloseDoorAt ) {
      // Door needs to be closed now
      activateDoorRelay();
      // Indicate that we are waiting for the door to close
      doorState = STATE_DOOR_CLOSING;
      // Allow DOOR_ALARM_SECONDS minute for the door to close before raising the alarm
      tCloseDoorAt = tNow + (1000L * DOOR_ALARM_SECONDS);
      cb(MSG_CLOSE_DOOR_NOW, 0);
    }
    else {
      // Display how long until door will close
      unsigned long countdown = tCloseDoorAt - tNow;
//#if DBG
//  Serial.print("countdown: ");
//  Serial.println(countdown);
//#endif
      cb(MSG_CLOSE_DOOR_COUNTDOWN, countdown);
    }    
  }
  else if ( doorState == STATE_DOOR_CLOSING ) {
    // Waiting for the door to close
    if ( tNow > tCloseDoorAt ) {
      // Time limit for closing the door has been surpassed.
      cb(MSG_DOOR_CLOSE_ERROR, 0);
    }
    else {
      // Waiting for the door to close
      unsigned long countdown = tCloseDoorAt - tNow;
      cb(MSG_DOOR_CLOSING, countdown);
    }
  }
}

boolean DoorController::isDoorOpen() {
  /* door mag switch is active low */
  door.update();
  return !door.read();
}

void DoorController::activateDoorRelay(){
  digitalWrite(doorRelay, HIGH);
  delay(doorRelayDelay);
  digitalWrite(doorRelay, LOW);
}


