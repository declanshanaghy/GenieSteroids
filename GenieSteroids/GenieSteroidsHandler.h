#ifndef GENIESTEROIDSHANDLER_H
#define GENIESTEROIDSHANDLER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "LcdMenuHandler.h"

class KeySoundHandler : public LcdMenuHandler {
public:
  KeySoundHandler(int ident) : LcdMenuHandler(ident) {};
  boolean procKeyPress(int k, char c);
  void dispayConfirmation(); 
};

class BootSoundHandler : public LcdMenuHandler {
public:
  BootSoundHandler(int ident) : LcdMenuHandler(ident) {};
//  boolean procKeyPress(int k, char c);
//  void dispayConfirmation(); 
};

#endif //GENIESTEROIDSHANDLER_H
