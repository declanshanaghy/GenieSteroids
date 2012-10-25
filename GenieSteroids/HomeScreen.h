#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Chronodot.h>

#define LOOP_UPDATE_INTERVAL 1000

#define TEMP_C 0
#define TEMP_F 1

#define LCD_CHAR_DEGREES 223

class HomeScreen {
public:
  HomeScreen(LiquidCrystal &lcd, Chronodot *chronodot, short pinTempSensor) 
    : lcd(lcd), chronodot(chronodot), pinTempSensor(pinTempSensor), 
      tLastDateTime(0), tLastTemp(0) {
      pinMode(pinTempSensor, INPUT); 
  };
  void loop();
  void display();
  
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
  unsigned long tLastDateTime;
  unsigned long tLastTemp;
  char sz_time[12];// = "00:00:00 AM";
  char sz_temp[5];// = "999F";
};

#endif //HOMESCREEN_H
