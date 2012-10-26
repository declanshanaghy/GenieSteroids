#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Chronodot.h>

#define LOOP_UPDATE_INTERVAL 1000

#define TEMP_C 0
#define TEMP_F 1

#define LCD_CHAR_DEGREES 223

#define COL_TIME 1
#define COL_TEMP 11

class HomeScreen {
public:
  HomeScreen(LiquidCrystal &lcd, Chronodot *chronodot, short pinTempSensor, boolean locked) 
    : lcd(lcd), chronodot(chronodot), pinTempSensor(pinTempSensor), locked(locked),
      tLastDateTime(0), tLastTemp(0) {
      pinMode(pinTempSensor, INPUT); 
  };
  void loop(unsigned long tNow);
  void reset() { tLastDateTime = 0; tLastTemp = 0; };
  void display();
  void setLocked(boolean locked) { this->locked = locked; displayHeader(); };
  boolean isLocked() { return locked; };
  
private:
  void displayHeader();
  void displayDateTime();
  void displayTemp();
  void displayTime(const int hours, const int minutes, const int seconds);
  void displayTemp(const int scale, const float temp);
  void readTime(int &hours, int &minutes, int &seconds);
  float readTemp(const int scale);
  float readTemp_Chronodot(const int scale);
  float readTemp_TMP36(const int scale);

  LiquidCrystal &lcd;
  Chronodot *chronodot;
  short pinTempSensor;
  boolean locked;
  unsigned long tLastDateTime;
  unsigned long tLastTemp;
  char sz_time[9];// = "00:00:00";
  char sz_temp[5];// = "999F";
};

#endif //HOMESCREEN_H
