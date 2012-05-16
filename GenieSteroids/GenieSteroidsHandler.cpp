
#include "GenieSteroidsHandler.h"

/********************************/
/*    GenericSoundHandler
/********************************/
boolean GenericSoundHandler::procKeyPress(int k, char c) {
  this->val = !this->val;
  return false;
}

void GenericSoundHandler::displayConfirmation() {
  if ( this->val ) {
    lcd->setCursor(5, 0);
    lcd->print("=-==-=");      
    lcd->setCursor(2, 1);
    lcd->print("  Sound on");
  }
  else {
    lcd->setCursor(5, 0);
    lcd->print("=-==-=");      
    lcd->setCursor(1, 1);
    lcd->print("   Sound off");      
  }
}

/********************************/
/*    DateTimeHandler
/********************************/
void DateTimeHandler::displayStart() {
  this->state = STATE_DT_DATE_M1;
  dt = RTC.now();
  displayDate();
}

boolean DateTimeHandler::procKeyPress(int k, char c) {
  this->val = !this->val;
  return true;
}

void DateTimeHandler::displayDate() {
    lcd->setCursor(5, 0);
    lcd->print("Set Date");      
    lcd->setCursor(2, 1);
    sprintf(sz_dt, "%2d/%2d/%4d", dt.month(), dt.day(), dt.year());
    lcd->print(sz_dt);  
}

