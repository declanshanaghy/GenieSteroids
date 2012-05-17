
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
        this->state--;
        break;
      case KEY_POUND:
        this->valid = true;
        this->state++;
        break;
      default:
        short v = (int)c - 48;
        short dec = 0;
        short unit = 0;
        short set = 0;
        switch ( this->state ) {
          case STATE_DT_DATE_M1:
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
              if (v >= 1 && v <= 9) {
                set = (dec * 10) + v;
                dt.setMonth(set);
                this->valid = true;
                this->state++;
              }
            }
            else {
              if (v >= 0 && v <= 2) {
                set = (dec * 10) + v;
                dt.setMonth(set);
                this->valid = true;
                this->state++;
              }
            }
            break;
          case STATE_DT_DATE_D1:
            break;
          case STATE_DT_DATE_D2:
            break;
          case STATE_DT_DATE_Y1:
            break;
          case STATE_DT_DATE_Y2:
            break;
          case STATE_DT_DATE_Y3:
            break;
          case STATE_DT_DATE_Y4:
            break;
          case STATE_DT_TIME_H1:
            break;
          case STATE_DT_TIME_H2:
            break;
          case STATE_DT_TIME_M1:
            break;
          case STATE_DT_TIME_M2:
            break;
          case STATE_DT_TIME_S1:
            break;
          case STATE_DT_TIME_S2:
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
  Serial.println("");
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

