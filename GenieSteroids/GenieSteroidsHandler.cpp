
#include "GenieSteroidsHandler.h"

/********************************/
/*    GenericSoundHandler
/********************************/
boolean GenericSoundHandler::procKeyPress(int k, char c) {
  this->val = !this->val;
  return false;
}

void GenericSoundHandler::displayConfirmation() {
  if ( this->val ) {
    lcd->setCursor(5, 0);
    lcd->print("=-==-=");      
    lcd->setCursor(2, 1);
    lcd->print("  Sound on");
  }
  else {
    lcd->setCursor(5, 0);
    lcd->print("=-==-=");      
    lcd->setCursor(1, 1);
    lcd->print("   Sound off");      
  }
}

/********************************/
/*    DateTimeHandler
/********************************/
void DateTimeHandler::displayStart() {
  this->state = STATE_DT_BEGIN;
  
  lcd->clear();
  lcd->blink();
  lcd->noCursor();
  dt = RTC.now();
  displayDate();
}

boolean DateTimeHandler::procKeyPress(int k, char c) {
//#if DBG
//  Serial.print("procKeyPress: ");
//  Serial.print("k=");
//  Serial.print(k);
//  Serial.print(", c=");
//  Serial.print(c);
//  Serial.println("");
//#endif

//#if DBG
//  Serial.print("state in: ");
//  Serial.print(state);
//#endif
  this->valid = false;
  boolean wasDate = this->state < STATE_DT_TIME_H1;

  if ( this->state == STATE_DT_BEGIN ) {
    this->state++;
    this->valid = true;
  }
  else {
    switch ( k ) {
      case KEY_STAR:
        this->valid = true;
        if ( this->state == STATE_DT_DATE_Y3 )
          // Automatically skip over century and millenimum input.
          // Ya ya, it's an intentional year 2100 bug :-P
          this->state -= 3;
        else
          this->state--;
        break;
      case KEY_POUND:
        this->valid = true;
        if ( this->state == STATE_DT_DATE_D2 )
          // Automatically skip over century and millenimum input.
          // Ya ya, it's an intentional year 2100 bug :-P
          this->state += 3;
        else
          this->state++;
        break;
      default:
        short month = dt.month();
        short days_in_month = (short)pgm_read_byte(DAYS_IN_MONTH + month - 1);
        short v = (int)c - 48;
        short dec = 0;
        short unit = 0;
        short set = 0;
        switch ( this->state ) {
          case STATE_DT_DATE_M1:
            // First digit of month can only be 0-1
            if (v >= 0 && v <= 1) {
              dec = dt.month() / 10;
              unit = dt.month() % 10;
              set = (10 * v) + unit;
              dt.setMonth(set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_DATE_M2:
            dec = dt.month() / 10;
            unit = dt.month() % 10;
            if ( dec == 0 ) {
              // If first digit of month is 0, 
              // second digit can be 1-9
              if (v >= 1 && v <= 9) {
                set = (dec * 10) + v;
                dt.setMonth(set);
                this->valid = true;
                this->state++;
              }
            }
            else {
              // If first digit of month is 1, 
              // second digit can be 0-2
              if (v >= 0 && v <= 2) {
                set = (dec * 10) + v;
                dt.setMonth(set);
                this->valid = true;
                this->state++;
              }
            }
            break;
          case STATE_DT_DATE_D1:
            // If month is 2 second digit can be 0-2
            // If month is not 2 second digit can be 0-3
            if ( (month == 2 && v >= 0 && v <= 2) || 
                 (month != 2 && v >= 0 && v <= 3) ) {
              dec = dt.day() / 10;
              unit = dt.day() % 10;
              set = (10 * v) + unit;
              dt.setDay(set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_DATE_D2:
            dec = dt.day() / 10;
            unit = dt.day() % 10;
            set = (dec * 10) + v;
            // Complete month digits must be between 1-12
            if ( set >= 1 && set <= days_in_month ) {
              dt.setDay(set);
              this->valid = true;
              // Automatically skip to decade input
              // Ya ya, it's an intentional year 2100 bug :-P
              this->state += 3;
            }
            break;
          case STATE_DT_DATE_Y1:
            break;
          case STATE_DT_DATE_Y2:
            break;
          case STATE_DT_DATE_Y3:
            // Ya ya, it's an intentional year 2100 bug :-P
            if ( v >= 0 && v <= 9 ) {
              dec = (dt.year() - 2000) / 10;
              unit = (dt.year() - 2000) % 10;
              set = (10 * v) + unit;
              dt.setYear(2000 + set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_DATE_Y4:
            // Ya ya, it's an intentional year 2100 bug :-P
            if ( v >= 0 && v <= 9 ) {
              dec = (dt.year() - 2000) / 10;
              unit = (dt.year() - 2000) % 10;
              set = (10 * dec) + v;
              dt.setYear(2000 + set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_TIME_H1:
            // First digit of hour can be 0-2
            if ( v >= 0 && v <= 2 ) {
              dec = dt.hour() / 10;
              unit = dt.hour() % 10;
              set = (10 * v) + unit;
              dt.setHour(set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_TIME_H2:
            dec = dt.hour() / 10;
            unit = dt.hour() % 10;
            set = (dec * 10) + v;
            // Complete hour digits must be between 0-23
            if ( set >= 0 && set <= 23 ) {
              dt.setHour(set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_TIME_M1:
            // First digit of minute can be 0-5
            if ( v >= 0 && v <= 5 ) {
              dec = dt.minute() / 10;
              unit = dt.minute() % 10;
              set = (10 * v) + unit;
              dt.setMinute(set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_TIME_M2:
            dec = dt.minute() / 10;
            unit = dt.minute() % 10;
            set = (dec * 10) + v;
            // Complete minute digits must be between 0-59
            if ( set >= 0 && set <= 59 ) {
              dt.setMinute(set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_TIME_S1:
            // First digit of second can be 0-5
            if ( v >= 0 && v <= 5 ) {
              dec = dt.second() / 10;
              unit = dt.second() % 10;
              set = (10 * v) + unit;
              dt.setSecond(set);
              this->valid = true;
              this->state++;
            }
            break;
          case STATE_DT_TIME_S2:
            dec = dt.second() / 10;
            unit = dt.second() % 10;
            set = (dec * 10) + v;
            // Complete second digits must be between 0-59
            if ( set >= 0 && set <= 59 ) {
              dt.setSecond(set);
              this->valid = true;
              this->state++;
            }
            break;
        }
#if DBG
  Serial.print("v=");
  Serial.print(v);
  Serial.print(", dec=");
  Serial.print(dec);
  Serial.print(", unit=");
  Serial.print(unit);
  Serial.print(", set=");
  Serial.print(set);
  Serial.print(", month=");
  Serial.print(month);
  Serial.print(", days_in_month=");
  Serial.print(days_in_month);
  Serial.print(", year=");
  Serial.print(dt.year());
  Serial.print(", valid=");
  Serial.print(valid);
  Serial.println("" ) ;
#endif        
    }
  }
  
//#if DBG
//  Serial.print("state out: ");
//  Serial.print(state);
//#endif

  boolean isDate = this->state < STATE_DT_TIME_H1;
  
  switch ( this->state ) {
    case STATE_DT_BEGIN:
      this-> confirmed = false;
      return false;
    case STATE_DT_CONFIRM:
      RTC.adjust(dt);
      this-> confirmed = true;
      return false;
    default:
      if ( isDate ) {
        if ( !wasDate )
          lcd->clear();
        displayDate();
      }
      else {
        if ( wasDate )
          lcd->clear();
        displayTime();
      }
      return true;
  }
}

void DateTimeHandler::displayDate() {
    short s = 3;
    
    lcd->setCursor(s, 0);
    lcd->print("MM/DD/YYYY");      
    lcd->setCursor(s, 1);
    sprintf(sz_dt, "%02d/%02d/%04d", dt.month(), dt.day(), dt.year());
    lcd->print(sz_dt);  
  
    switch ( this->state ) {
      case STATE_DT_DATE_M1:
        lcd->setCursor(s+0, 1);
        break;
      case STATE_DT_DATE_M2:
        lcd->setCursor(s+1, 1);
        break;
      case STATE_DT_DATE_D1:
        lcd->setCursor(s+3, 1);
        break;
      case STATE_DT_DATE_D2:
        lcd->setCursor(s+4, 1);
        break;
      case STATE_DT_DATE_Y1:
        lcd->setCursor(s+6, 1);
        break;
      case STATE_DT_DATE_Y2:
        lcd->setCursor(s+7, 1);
        break;
      case STATE_DT_DATE_Y3:
        lcd->setCursor(s+8, 1);
        break;
      case STATE_DT_DATE_Y4:
        lcd->setCursor(s+9, 1);
        break;
    }
}

void DateTimeHandler::displayTime() {
    short s = 4;

    lcd->setCursor(s, 0);
    lcd->print("HH:MM:SS");      
    lcd->setCursor(s, 1);
    sprintf(sz_dt, "%02d:%02d:%02d", dt.hour(), dt.minute(), dt.second());
    lcd->print(sz_dt);  
  
    switch ( this->state ) {
      case STATE_DT_TIME_H1:
        lcd->setCursor(s+0, 1);
        break;
      case STATE_DT_TIME_H2:
        lcd->setCursor(s+1, 1);
        break;
      case STATE_DT_TIME_M1:
        lcd->setCursor(s+3, 1);
        break;
      case STATE_DT_TIME_M2:
        lcd->setCursor(s+4, 1);
        break;
      case STATE_DT_TIME_S1:
        lcd->setCursor(s+6, 1);
        break;
      case STATE_DT_TIME_S2:
        lcd->setCursor(s+7, 1);
        break;
    }
}

