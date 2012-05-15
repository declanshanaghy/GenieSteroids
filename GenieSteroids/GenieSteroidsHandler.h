#ifndef GENIESTEROIDSHANDLER_H
#define GENIESTEROIDSHANDLER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "LcdMenuHandler.h"

class GenericSoundHandler : public LcdMenuHandler {
public:
  GenericSoundHandler(int ident) : LcdMenuHandler(ident) {};
  virtual boolean procKeyPress(int k, char c);
  virtual void dispayConfirmation(void);
};

#endif //GENIESTEROIDSHANDLER_H
