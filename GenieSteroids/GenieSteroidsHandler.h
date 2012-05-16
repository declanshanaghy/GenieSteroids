#ifndef GENIESTEROIDSHANDLER_H
#define GENIESTEROIDSHANDLER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "LcdMenuHandler.h"
#include <Chronodot.h>

class GenericSoundHandler : public LcdMenuHandler {
public:
  GenericSoundHandler(int ident) : LcdMenuHandler(ident) {};
  boolean procKeyPress(int k, char c);
  void displayConfirmation(void);
};

#define STATE_DT_DATE_M1  0
#define STATE_DT_DATE_M2  1
#define STATE_DT_DATE_D1  2
#define STATE_DT_DATE_D2  3
#define STATE_DT_DATE_Y1  4
#define STATE_DT_DATE_Y2  5
#define STATE_DT_DATE_Y3  6
#define STATE_DT_DATE_Y4  7

#define STATE_DT_TIME_H1  0
#define STATE_DT_TIME_H2  0
#define STATE_DT_TIME_M1  0
#define STATE_DT_TIME_M2  0

class DateTimeHandler : public LcdMenuHandler {
public:
  DateTimeHandler(int ident) : LcdMenuHandler(ident) {};
  virtual short getValueType() { return TYPE_LONG; };
  void displayStart();
  boolean procKeyPress(int k, char c);
private:
  Chronodot RTC;
  DateTime dt;
  char sz_dt[11];

  void displayDate();
};

#endif //GENIESTEROIDSHANDLER_H
