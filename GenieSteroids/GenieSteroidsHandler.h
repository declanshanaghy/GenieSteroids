#ifndef GENIESTEROIDSHANDLER_H
#define GENIESTEROIDSHANDLER_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Chronodot.h>
#include <AnalogKeypad.h>
#include "LcdMenuHandler.h"

#define INTERVAL_STATE_BEGIN 0
#define INTERVAL_STATE_FIRST_CHAR 1
#define INTERVAL_MAX_CHARS 3
#define INTERVAL_STATE_END INTERVAL_STATE_FIRST_CHAR + INTERVAL_MAX_CHARS

#define DBG 0

class GenericSoundHandler : public LcdMenuHandler {
public:
  GenericSoundHandler(int ident) : LcdMenuHandler(ident) {};
  boolean procKeyPress(int k, char c);
  void displayConfirmation(void);
};

class IntervalHandler : public LcdMenuHandler {
private:
  String interval;

private:  
  void displayInterval();
  void clearBottomRow();
  void storeValue();
  
public:
  IntervalHandler(int ident) : LcdMenuHandler(ident) {};
  virtual short getValueType() { return TYPE_LONG; };
  void displayStart();
  boolean procKeyPress(int k, char c);
};


static prog_uchar DAYS_IN_MONTH[] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

class DateTimeHandler : public LcdMenuHandler {
public:
  DateTimeHandler(int ident) : LcdMenuHandler(ident) {};
  virtual short getValueType() { return TYPE_LONG; };
  DateTime getDateTime() { return dt; };
  
protected:
  virtual void setConfirmed() { confirmed = true; };

  Chronodot chronodot;
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
  virtual void displayStart();
  
protected:
  void displayDate();  
};

class ChronodotDateHandler : public DateHandler {
public:
  ChronodotDateHandler(int ident) : DateHandler(ident) {};
  virtual void displayStart();
  
protected:
  virtual void setConfirmed();
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
  virtual void displayStart();
  
protected:
  virtual void setConfirmed();
  void displayTime();
};

class ChronodotTimeHandler : public TimeHandler {
public:
  ChronodotTimeHandler(int ident) : TimeHandler(ident) {};
  virtual void displayStart();
  
protected:
  virtual void setConfirmed();
};

class TimeOfDayHandler : public TimeHandler {
public:
  TimeOfDayHandler(int ident) : TimeHandler(ident) {};
  virtual void displayStart();
  virtual void setValue(long value);
};

  
#endif //GENIESTEROIDSHANDLER_H
