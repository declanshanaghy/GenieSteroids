#ifndef GENIESTEROIDSHANDLER_H
#define GENIESTEROIDSHANDLER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Chronodot.h>
#include <AnalogKeypad.h>
#include "LcdMenuHandler.h"

#define DBG 1

class GenericSoundHandler : public LcdMenuHandler {
public:
  GenericSoundHandler(int ident) : LcdMenuHandler(ident) {};
  boolean procKeyPress(int k, char c);
  void displayConfirmation(void);
};

static prog_uchar DAYS_IN_MONTH[] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

#define STATE_DT_BEGIN    0
#define STATE_DT_DATE_M1  1
#define STATE_DT_DATE_M2  2
#define STATE_DT_DATE_D1  3
#define STATE_DT_DATE_D2  4
#define STATE_DT_DATE_Y1  5
#define STATE_DT_DATE_Y2  6
#define STATE_DT_DATE_Y3  7
#define STATE_DT_DATE_Y4  8

#define STATE_DT_TIME_H1  9
#define STATE_DT_TIME_H2  10
#define STATE_DT_TIME_M1  11
#define STATE_DT_TIME_M2  12
#define STATE_DT_TIME_S1  13
#define STATE_DT_TIME_S2  14
#define STATE_DT_CONFIRM  15

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
  void displayTime();
};

#endif //GENIESTEROIDSHANDLER_H
