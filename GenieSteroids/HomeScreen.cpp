
#include "HomeScreen.h"

void HomeScreen::loop(unsigned long tNow) {
#if DBG
  Serial.print("HomeScreen::loop: ");
  Serial.println(tNow);
#endif
  if (tLastDateTime == 0 || tNow - tLastDateTime > LOOP_UPDATE_INTERVAL) {
    displayDateTime();
    tLastDateTime = tNow;
  }
  
  if (tLastTemp == 0 || tNow - tLastTemp > (LOOP_UPDATE_INTERVAL * 10)) {
    displayTemp();
    tLastTemp = tNow;
  }
}

void HomeScreen::display() {
  lcd.clear();
  lcd.noBlink();
  lcd.noCursor();
  displayHeader();
  displayDateTime();
  displayTemp();
}

void HomeScreen::displayHeader() {
  lcd.home();
  if (locked)
    lcd.print("   LOCKED :-)   ");
  else
    lcd.print(" GenieSteroids! ");
}

void HomeScreen::displayDateTime() {
#if DBG
  Serial.print("HomeScreen::displayDateTime: ");
  Serial.println(tNow);
#endif
  int hours=0, minutes=0, seconds=0;

  readTime(hours, minutes, seconds);
  displayTime(hours, minutes, seconds);
}

void HomeScreen::displayTemp() {
#if DBG
  Serial.print("HomeScreen::displayTemp: ");
  Serial.println(tNow);
#endif
  float temp = readTemp(TEMP_F);
  displayTemp(TEMP_F, temp);
}

void HomeScreen::displayTime(const int hours, const int minutes, const int seconds) {
  sprintf(sz_time, "%02d:%02d:%02d", hours, minutes, seconds);
//#if DBG
//  Serial.print("Time: '"); Serial.println(sz_time);
//#endif
  
  lcd.setCursor(COL_TIME, 1);
  lcd.print(sz_time);
}

void HomeScreen::displayTemp(const int scale, const float temp) {
//#if DBG
//  Serial.print(temperatureF); Serial.println(" degress F");
//#endif
  sprintf(sz_temp, "%3d%c", int(temp), LCD_CHAR_DEGREES);
//#if DBG
//  Serial.print("TEMP: "); Serial.println(sz_temp);
//#endif
  
  lcd.setCursor(COL_TEMP, 1);
  lcd.print(sz_temp);
}

void HomeScreen::readTime(int &hours, int &minutes, int &seconds) {
  DateTime now = chronodot->now();
  hours = now.hour();
  minutes = now.minute();
  seconds = now.second();
}

float HomeScreen::readTemp(const int scale) {
  return readTemp_TMP36(scale);
}

float HomeScreen::readTemp_Chronodot(const int scale) {
  if ( scale == TEMP_F ) {
    return chronodot->now().tempF();
  }
  else {
    return chronodot->now().tempC();
  }
}

float HomeScreen::readTemp_TMP36(const int scale) {
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(pinTempSensor);  
   
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
   
  // print out the voltage
//#if DBG
//  Serial.print(voltage); Serial.println(" volts");
//#endif
   
  // now print out the temperature
  float t = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                     //to degrees ((volatge - 500mV) times 100)
//#if DBG
//  Serial.print(t); Serial.println(" degress C");
//#endif
  
  if ( scale == TEMP_F ) {
    // now convert to Fahrenheight
    t = (t * 9.0 / 5.0) + 32.0;
//#if DBG
//    Serial.print(t); Serial.println(" degress C");
//#endif
  }
  return t;
}

