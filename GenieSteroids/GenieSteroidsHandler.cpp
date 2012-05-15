
#include "GenieSteroidsHandler.h"

/********************************/
/*    GenericSoundHandler
/********************************/
boolean GenericSoundHandler::procKeyPress(int k, char c) {
  this->val = !this->val;
  return false;
}

void GenericSoundHandler::dispayConfirmation() {
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

