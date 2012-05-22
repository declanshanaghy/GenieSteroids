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


class DateTimeHandler : public LcdMenuHandler {
public:
  DateTimeHandler(int ident) : LcdMenuHandler(ident) {};
  short getValueType() { return TYPE_LONG; };
protected:
  Chronodot RTC;
  DateTime dt;
  char sz_dt[11];
};

#define STATE_DT_DATE_BEGIN   0
#define STATE_DT_DATE_M1      1
#define STATE_DT_DATE_M2      2
#define STATE_DT_DATE_D1      3
#define STATE_DT_DATE_D2      4
#define STATE_DT_DATE_Y1      5
#define STATE_DT_DATE_Y2      6
#define STATE_DT_DATE_Y3      7
#define STATE_DT_DATE_Y4      8
#define STATE_DT_DATE_CONFIRM 9
class DateHandler : public DateTimeHandler {
public:
  DateHandler(int ident) : DateTimeHandler(ident) {};
  boolean procKeyPress(int k, char c);
  void displayStart();
  void displayDate();
};

#define STATE_DT_TIME_BEGIN   0
#define STATE_DT_TIME_H1      1
#define STATE_DT_TIME_H2      2
#define STATE_DT_TIME_M1      3
#define STATE_DT_TIME_M2      4
#define STATE_DT_TIME_S1      5
#define STATE_DT_TIME_S2      6
#define STATE_DT_TIME_CONFIRM 7
class TimeHandler : public DateTimeHandler {
public:
  TimeHandler(int ident) : DateTimeHandler(ident) {};
  boolean procKeyPress(int k, char c);
  void displayStart();
  void displayTime();
};

#endif //GENIESTEROIDSHANDLER_H
