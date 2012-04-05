
#include "GenieSteroidsHandler.h"

/********************************/
/*    KeySoundHandler
/********************************/
boolean KeySoundHandler::procKeyPress(int k, char c) {
  this->val = !this->val;
  return false;
}

void KeySoundHandler::dispayConfirmation() {
  if ( this->val ) {
    lcd->setCursor(5, 0);
    lcd->print("=-==-=");      
    lcd->setCursor(2, 1);
    lcd->print("Key sounds on");      
  }
  else {
    lcd->setCursor(5, 0);
    lcd->print("=-==-=");      
    lcd->setCursor(1, 1);
    lcd->print("Key sounds off");      
  }
}

/********************************/
/*    BootSoundHandler
/********************************/
//boolean BootSoundHandler::procKeyPress(int k, char c) {
//  this->val = !this->val;
//  return false;
//}
//
//void BootSoundHandler::dispayConfirmation() {
//  if ( this->val ) {
//    lcd->setCursor(5, 0);
//    lcd->print("=-==-=");      
//    lcd->setCursor(2, 1);
//    lcd->print("Boot sound on");      
//  }
//  else {
//    lcd->setCursor(5, 0);
//    lcd->print("=-==-=");      
//    lcd->setCursor(1, 1);
//    lcd->print("Boot sound off");      
//  }
//}

