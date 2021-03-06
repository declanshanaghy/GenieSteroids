#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal.h>

#include <Chronodot.h>
#include <Bounce.h>

#include <AnalogKeypad.h>
#include <LcdMenu.h>

#include "GeniePrefs.h"
#include "GenieSteroidsHandler.h"
#include "DoorController.h"
#include "HomeScreen.h"
#include "LockManager.h"

#define DBG 0

/***************************
  ANALOG PIN DEFINIDTIONS
****************************/
#define SENSOR_KEYS    A0
#define SENSOR_TEMP    A1
#define SENSOR_LIGHT   A2

/***************************
  DIGITAL PIN DEFINIDTIONS
****************************/
#define PIN_DOOR_SENS  2
#define RELAY_LOCK     5
#define RELAY_LIGHT    6
#define RELAY_DOOR     7
#define PIN_BUZZ       8

#define PIN_LCD_BL_PWR   3
#define PIN_LCD_RS      13
#define PIN_LCD_EN       4
#define PIN_LCD_D4      12
#define PIN_LCD_D5      11
#define PIN_LCD_D6      10
#define PIN_LCD_D7       9

/***************************
  KEYPAD
****************************/
#define MENU_1  '1'
#define MENU_2  '2'
#define MENU_3  '3'
#define MENU_4  '4'
#define MENU_5  '5'
#define MENU_6  '6'
#define MENU_7  '7'
#define MENU_8  '8'
#define MENU_9  '9'

/***************************
  STATES
****************************/
#define STATE_IDLE 0
#define STATE_MENU 1
#define STATE_HANDLER 2

/***************************
  OTHER CONSTANTS
****************************/
#define MAX_ULONG 4294967295L

#define LCD_COLS 16
#define LCD_ROWS 2

#define INTERVAL_LIGHT_UPDATE 100    // How oftent o sample the light reading
#define N_LIGHT_VALUES 10            // Number of light samples to average
#define DOOR_ALARM_NOTIFY 10         // Seconds of notification before closing the door
#define RELAY_DELAY 250              // Hold the relay signal for this long
#define INPUT_IDLE_TIMEOUT 60000L    // How long between keypresses until we go back to idle state
#define INPUT_IDLE_LCD_OFF 300000L   // How long between keypresses until the LCD is disabled

/*****************************
  GLOBAL VARS
******************************/
unsigned long tNow = 0;  // Tracks current cpu time
boolean lcdOn = false;
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D7, 
                  PIN_LCD_D6, PIN_LCD_D5, PIN_LCD_D4);
AnalogKeypad *kpad = new AnalogKeypad(SENSOR_KEYS, REPEAT_OFF, 10000, 1000);
Chronodot *chronodot = new Chronodot();
GeniePrefs *prefs = new GeniePrefs();
HomeScreen *homeScreen = new HomeScreen(lcd, chronodot, SENSOR_TEMP, false);
DoorController *doorCtrl = new DoorController(&doorControllerCallback, prefs, PIN_DOOR_SENS, RELAY_DOOR, RELAY_DELAY);
LockManager *lockMgr = new LockManager(&lockMgrCallback);

short light_values[N_LIGHT_VALUES] = {255};
int light_total = 0;
short light_idx = 0;
short light_avg = 0;

unsigned long tLastLightUpdate = 0;
unsigned long tLastActivity = 0;

short state = STATE_IDLE;

LcdMenuHandler* currentHandler;
GenericSoundHandler hdlrKeySound(CFG_KEY_SOUND);
GenericSoundHandler hdlrBootSound(CFG_BOOT_SOUND);
GenericSoundHandler hdlrOtherSound(CFG_OTHER_SOUND);
ChronodotDateHandler hdlrDate(CFG_UNUSED);
ChronodotTimeHandler hdlrTime(CFG_UNUSED);
IntervalHandler hdlrOpenDuration(CFG_OPEN_DURATION);
TimeOfDayHandler hdlrLock1(CFG_LOCK1);
TimeOfDayHandler hdlrUnlock1(CFG_UNLOCK1);

LcdMenu menu(&lcd, LCD_COLS, LCD_ROWS);
LcdMenuEntry mOpenDuration(MENU_1, "Close Timer", &hdlrOpenDuration);
LcdMenuEntry mLock1(MENU_2, "Set Lock", &hdlrLock1);
LcdMenuEntry mUnlock1(MENU_3, "Set Unlock", &hdlrUnlock1);
LcdMenuEntry mDate(MENU_4, "Set Date", &hdlrDate);
LcdMenuEntry mTime(MENU_5, "Set Time", &hdlrTime);
LcdMenuEntry mSounds(MENU_6, "Sounds", NULL);

LcdMenuEntry mKeySound(MENU_1, "Key Press", &hdlrKeySound);
LcdMenuEntry mBootSound(MENU_2, "Boot Up", &hdlrBootSound);
LcdMenuEntry mOtherSound(MENU_3, "Confirmations", &hdlrOtherSound);

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void setup() {
  Wire.begin();
#if DBG
  Serial.begin(115200);
  Serial.println("SETUP");
#endif

  state = STATE_IDLE;
  
  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_LOCK, OUTPUT); 
  pinMode(PIN_BUZZ, OUTPUT); 

  kpad->init(); 
  setupChronoDot();
  setupPrefs();
  setupLCD();  
  setupMenu();
  unlockDoor();
  
  toneBoot();
  //setupFakeLocks();
}

//void setupFakeLocks() {
//  DateTime fake = chronodot->now();
//  fake.setHour(23);
//  fake.setMinute(59);
//  fake.setSecond(50);  
//  chronodot->adjust(fake);
//  
//  DateTime lock = fake;
//  lock.setHour(23);
//  lock.setMinute(59);
//  lock.setSecond(52);  
//  
//  DateTime unlock = fake;
//  unlock.setHour(23);
//  unlock.setMinute(59);
//  unlock.setSecond(58);  
//  
//  lockMgr->setEvents(lock, unlock) ;
//}

void loop() {
  unsigned long _tNow = millis();
  if ( _tNow < tNow  ) // millis has rolled over (approximately every 50 days)
    rollover();
  tNow = _tNow;
  
//#if DBG
//  Serial.print("tNow: ");
//  Serial.println(tNow);
//#endif

  lockMgr->loop(tNow);
  doorCtrl->loop(tNow);
  procLoopKeyPress(tNow);
  procLoopState(tNow);
}

void rollover() {
  tLastLightUpdate = 0;
  recordActivity(false);
  lockMgr->reset();
  doorCtrl->reset();
  homeScreen->reset();
}

void lockMgrCallback(short msg, unsigned long countdown) {
//#if DBG
//  Serial.print("MSG (rx): ");
//  Serial.println(msg);
//#endif
  
  switch ( msg ) {
  case MSG_LOCK_NOW:
    toneNotify();
    lockDoor();
    break;
  case MSG_UNLOCK_NOW:
    toneNotify();
    unlockDoor();
    break;
  }
}

void recordActivity(boolean enableLcd) {
  tLastActivity = tNow;
  if ( enableLcd && !lcdOn )
    enableLCD();
}

void doorControllerCallback(short msg, unsigned long countdown) {
  countdown /= 1000;
  char seconds[2];
  
  recordActivity(true);
  
  switch ( msg ) {
  case MSG_DOOR_OPEN:
    if ( state == STATE_IDLE ) {      
      lcd.home();
      lcd.print("      OPEN      ");
    }
    break;
  case MSG_DOOR_CLOSED:
    homeScreen->display();
    break;
  case MSG_CLOSE_DOOR_COUNTDOWN:
    if ( state == STATE_IDLE ) {
      lcd.home();
      lcd.print("  Close:        ");
      lcd.setCursor(9, 0);
      lcd.print(countdown/60);
      lcd.print(":");
      sprintf(seconds, "%02d", countdown % 60);
      lcd.print(seconds);
      
      if ( countdown < DOOR_ALARM_NOTIFY )
        toneNotify();
    }
    break;
  case MSG_CLOSE_DOOR_NOW:
    if ( state == STATE_IDLE ) {
      lcd.home();
      lcd.print("    Closing     ");
    }
    break;
  case MSG_DOOR_CLOSE_ERROR:
    if ( state == STATE_IDLE ) {
      lcd.home();
      lcd.print(" CANT CLOSE :-( ");
    }
    toneAlarm();
    break;
  }
}

void setupPrefs() {
  prefs->load();
  hdlrKeySound.setValue(prefs->readBoolean(hdlrKeySound.getIdent(), KEY_SOUND_DEFAULT));
  hdlrBootSound.setValue(prefs->readBoolean(hdlrBootSound.getIdent(), BOOT_SOUND_DEFAULT));
  hdlrOtherSound.setValue(prefs->readBoolean(hdlrOtherSound.getIdent(), OTHER_SOUND_DEFAULT));
  hdlrOpenDuration.setValue(prefs->readInt(hdlrOpenDuration.getIdent(), OPEN_DURATION_DEFAULT));
  hdlrLock1.setValue(prefs->readLong(hdlrLock1.getIdent(), LOCK1_DEFAULT));  
  hdlrUnlock1.setValue(prefs->readLong(hdlrUnlock1.getIdent(), UNLOCK1_DEFAULT));
  
  lockMgr->setEvents(hdlrLock1.getDateTime(), hdlrUnlock1.getDateTime());
}

void setupMenu() {
  menu.setHead(&mOpenDuration);
  mOpenDuration.appendSibling(&mLock1);
  mLock1.appendSibling(&mUnlock1);
  mUnlock1.appendSibling(&mDate);
  mDate.appendSibling(&mTime);
  mTime.appendSibling(&mSounds);
  
  mSounds.setChild(&mKeySound);
  mKeySound.appendSibling(&mBootSound);
  mBootSound.appendSibling(&mOtherSound);
    
  currentHandler = NULL;
}

void toneBoot() {
  if ( prefs->bootSound )
    tone(PIN_BUZZ, 4000, 50);
}

void toneKey() {
  if ( prefs->keySounds )
    tone(PIN_BUZZ, 3000, 10);
}

void toneConfirm() {
  if ( prefs->otherSounds ) {
    tone(PIN_BUZZ, 1000, 100);
    delay(100);
    tone(PIN_BUZZ, 2000, 200);
  }
}

void toneCancel() {
  if ( prefs->otherSounds ) {
    tone(PIN_BUZZ, 1000, 100);
    delay(100);
    tone(PIN_BUZZ, 500, 200);
  }
}

void toneNotify() {
  tone(PIN_BUZZ, 2000, 200);
}

void toneAlarm() {
  tone(PIN_BUZZ, 1000, 100);
  delay(100);
  tone(PIN_BUZZ, 500, 200);
}

void toneInvalid() {
  tone(PIN_BUZZ, 75, 50);
}

void setupLCD() {  
  // set up the LCD's number of columns and rows: 
  lcd.begin(LCD_COLS, LCD_ROWS);

  pinMode(PIN_LCD_BL_PWR, OUTPUT); 
  enableLCD();
  
  homeScreen->display();
}

void setupChronoDot() {
  chronodot->begin();

  // If the chronodot loses power it will indicate that via isrunning()
  if (! chronodot->isrunning()) {
    // TODO: Set time to midnight and trigger alarm so user knows the time needs to be set.
    
    // The following line sets the chronodot to the date & time this sketch was compiled
    DateTime COMPILE_TIME = DateTime(__DATE__, __TIME__);
    chronodot->adjust(COMPILE_TIME);
  }
}

void procLoopState(unsigned long tNow) {
  switch (state) {
    case STATE_IDLE:
      homeScreen->loop(tNow);
      break;
  }
  
  if (state != STATE_IDLE && tNow - tLastActivity > INPUT_IDLE_TIMEOUT) {
    setIdle();
  }
    
  if (state == STATE_IDLE && tNow - tLastActivity > INPUT_IDLE_LCD_OFF) {
//#if DBG
//    Serial.println("LCD timeout");
//#endif
    disableLCD();    
  }
  else {
    //Things to do in every state
    adjustBacklight(tNow);
  }
}

void setIdle() {
  state = STATE_IDLE;
  menu.reset();
  homeScreen->display();
}

void procLoopKeyPress(unsigned long tNow) {  
  int k = kpad->readKey();
  char c = kpad->getLastKeyChar();
    
  if ( k != KEY_NONE ) {
//#if DBG
//  Serial.print("procLoopKeyPress: ");
//  Serial.println(k);
//#endif
    if ( !lcdOn )
      enableLCD();      
      toneKey();
      
//#if DBG
//    Serial.print("Menu key: code="); Serial.println(k);
//    Serial.print("          char="); Serial.println(c);
//#endif
    
    recordActivity(true);
    
    switch (state) {
      case STATE_IDLE:
        if ( k == KEY_POUND || k == KEY_STAR )
          displayMenu();
        break;
      case STATE_MENU:
        procLoopStateMenu(k, c);
        break;
      case STATE_HANDLER:
        procLoopStateHandler(k, c);
        break;
    }
  }
}

void procLoopStateHandler(int k, char c) {
  if ( currentHandler != NULL && currentHandler->wantsControl() ) {
//#if DBG
//      Serial.print("handle key: ");
//      Serial.print("k=");
//      Serial.print(k);
//      Serial.print(", c=");
//      Serial.print(c);
//      Serial.println("");
//#endif
    // procKeyPress returns false when the handler is relinquishing control.
    if (! currentHandler->procKeyPress(k, c)) {
      //isConfirmed indicates whether the action was confirmed or canceled
      clearHandler(currentHandler->isConfirmed());
    }
    else {
      if ( !currentHandler->isValid() )  //User entered invalid keypress
        toneInvalid();
    }
//    else {
//#if DBG
//        Serial.println("Handler retained control");
//#endif
//    }
  }
}

void clearHandler(boolean confirmed) {
  lcd.clear();
  lcd.noBlink();
  lcd.noCursor();

  if ( confirmed ) {
//#if DBG
//    Serial.println("Handler relinquished control");
//    Serial.print("Saving value: ");
//    Serial.println(currentHandler->getValue());
//#endif
    switch ( currentHandler->getValueType() ) {
      case TYPE_SHORT:
        prefs->writeShort(currentHandler->getIdent(), currentHandler->getValue());
        break;
      case TYPE_INT:
        prefs->writeInt(currentHandler->getIdent(), currentHandler->getValue());
        break;
      case TYPE_LONG:
        prefs->writeLong(currentHandler->getIdent(), currentHandler->getValue());
        break;
//#if DBG
//      default:
//        Serial.println("*** No value saved ***");
//#endif
    }

    setupPrefs();
    toneConfirm();  
    currentHandler->displayConfirmation();
  }  
  else {
//#if DBG
//    Serial.println("Canceling handler");
//#endif
    toneCancel();
    currentHandler->displayCancellation();
  }  

  currentHandler->relinquishControl();
  currentHandler = NULL;  
  
  delay(500);
  displayMenu();
}

void procLoopStateMenu(int k, char c) {
  if ( k != KEY_NONE ) {
    switch (k) {
      case KEY_STAR:
        menu.pageReverse();
        displayMenu();
        break;
      case KEY_0:
        if ( menu.levelUp() )
          displayMenu();
        else
          setIdle();
        break;
      case KEY_POUND:
        menu.page();
        displayMenu();
        break;
      default:
        LcdMenuHandler* h = menu.procKeyPress(c);
        if ( h != NULL ) {
          currentHandler = h;
//#if DBG
//          Serial.print("Assigned handler: "); Serial.println((int)currentHandler);
//#endif
          state = STATE_HANDLER;
          currentHandler->takeControl(&lcd);
          currentHandler->displayStart();
          procLoopStateHandler(k, c);
        }
        break;
    }
  }
}

void displayMenu() {
  menu.display();
  state = STATE_MENU;
}

void lockDoor(){
  homeScreen->setLocked(true);
  digitalWrite(RELAY_LOCK, HIGH);
}

void unlockDoor(){
  homeScreen->setLocked(false);
  digitalWrite(RELAY_LOCK, LOW);
}

void activateLightRelay(){
  digitalWrite(RELAY_LIGHT, HIGH);
  delay(RELAY_DELAY);
  digitalWrite(RELAY_LIGHT, LOW);
}

void adjustBacklight(unsigned long tNow) {  
  if ( lcdOn && (tLastLightUpdate == 0 || tNow > tLastLightUpdate + INTERVAL_LIGHT_UPDATE) ) {
//#if DBG
//  Serial.println("adjustBacklight");
//#endif
    //get the voltage reading from the light sensor
    int v = analogRead(SENSOR_LIGHT);
    light_values[light_idx] = map(v, 0, 1024, 16, 255);
  
    // Add the new value to the total
    light_total += light_values[light_idx];
    
    light_avg = light_total / N_LIGHT_VALUES;
    analogWrite(PIN_LCD_BL_PWR, light_avg);
  
//#if DBG
//  Serial.print("LIGHT: ");
//  for (int i=0; i<N_LIGHT_VALUES; i++) {
//    Serial.print(light_values[i]);
//    Serial.print(' ');
//  }
//  Serial.println();
//  Serial.print("light_total: ");
//  Serial.print(light_total);
//  Serial.print(", light_avg: ");
//  Serial.println(light_avg);
//#endif
  
    //  Move the index
    light_idx++;
    if ( light_idx == N_LIGHT_VALUES )
      light_idx = 0;
    
    // Subtract the current value from the total
    light_total -= light_values[light_idx];
    
    tLastLightUpdate = tNow;
  }
}

void enableLCD() {
  if ( !lcdOn ) {
//#if DBG
//  Serial.println("enableLCD");
//#endif
    lcdOn = true;
    lcd.display();
    analogWrite(PIN_LCD_BL_PWR, light_avg);
  }
}

void disableLCD() {
  if ( lcdOn ) {
//#if DBG
//  Serial.println("disableLCD");
//#endif
    lcdOn = false;
    lcd.noDisplay();
    digitalWrite(PIN_LCD_BL_PWR, LOW);
  }
}

