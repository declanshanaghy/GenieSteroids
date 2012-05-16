#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal.h>

#include <Chronodot.h>
#include <Bounce.h>

#include <AnalogKeypad.h>
#include <LcdMenu.h>
//#include <avr/pgmspace.h>

#include "GeniePrefs.h"
#include "GenieSteroidsHandler.h"

//#define DBG 0

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
#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_CHAR_DEGREES 223

#define RELAY_DELAY 200

#define TEMP_C 0
#define TEMP_F 1

#define IDLE_LOOP_UPDATE 500
#define INPUT_IDLE_TIMEOUT 10000

/*****************************
  PROGRAM MEMORY
******************************/
//PROGMEM prog_char string_override[] = "Override Open";
//PROGMEM prog_char string_close[] = "Close Timer";
//PROGMEM prog_char string_autolock[] = "Auto Lock";
//PROGMEM prog_char string_settings[] = "Settings";
//PROGMEM prog_char string_lock1[] = "Lock 1";
//PROGMEM prog_char string_lock2[] = "Lock 2";
//PROGMEM prog_char string_openduration[] = "Open Duration";
//PROGMEM prog_char string_datetime[] = "Date & Time";
//PROGMEM prog_char string_backlight[] = "Backlight";
//PROGMEM prog_char string_sounds[] = "Sounds";
//PROGMEM prog_char string_keypress[] = "Key Press";
//PROGMEM prog_char string_bootsound[] = "Boot Sound";
//PROGMEM prog_char string_menusounds[] = "Menu Sounds";

/*****************************
  GLOBAL VARS
******************************/
const DateTime COMPILE_TIME = DateTime(__DATE__, __TIME__);

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D7, 
                  PIN_LCD_D6, PIN_LCD_D5, PIN_LCD_D4);
AnalogKeypad kpad = AnalogKeypad(SENSOR_KEYS, REPEAT_OFF, 10000, 1000);
Chronodot RTC;
GeniePrefs prefs;
Bounce door(PIN_DOOR_SENS, 100);
boolean doorOpen = false;

char sz_time[] = "00:00:00 AM";
char sz_temp[] = "999F";

int light = 0;
long tNow = 0;
long tLastIdleReading = 0;
long tLastKeyPress = 0;

short state = STATE_IDLE;

LcdMenuHandler* currentHandler;
GenericSoundHandler hdlrKeySound(CFG_KEY_SOUND);
GenericSoundHandler hdlrBootSound(CFG_BOOT_SOUND);
GenericSoundHandler hdlrOtherSound(CFG_OTHER_SOUND);
DateTimeHandler hdlrDateTime(CFG_UNUSED);

LcdMenu menu(&lcd, LCD_COLS, LCD_ROWS);
LcdMenuEntry mOverrideOpen(MENU_1, "Override Open", NULL);
LcdMenuEntry mAutoClose(MENU_2, "Close Timer", NULL);
LcdMenuEntry mLockTimes(MENU_3, "Auto Lock", NULL);
LcdMenuEntry mSettings(MENU_4, "Settings", NULL);

LcdMenuEntry mLock1(MENU_1, "Lock 1", NULL);
LcdMenuEntry mLock2(MENU_2, "Lock 2", NULL);

LcdMenuEntry mOpenDuration(MENU_1, "Open Duration   ", NULL);
LcdMenuEntry mDate(MENU_2, "Date & Time", &hdlrDateTime);
LcdMenuEntry mBacklight(MENU_3, "Backlight", NULL);
LcdMenuEntry mSounds(MENU_4, "Sounds", NULL);

LcdMenuEntry mKeySound(MENU_1, "Key Press", &hdlrKeySound);
LcdMenuEntry mBootSound(MENU_2, "Boot Up", &hdlrBootSound);
LcdMenuEntry mOtherSound(MENU_3, "Confirmations", &hdlrOtherSound);

void setup() {
  Wire.begin();
  Serial.begin(115200);

//#if DBG
//  Serial.println("setup()");  
//#endif

  state = STATE_IDLE;
  
  kpad.init();
  
  pinMode(PIN_DOOR_SENS, INPUT); 
  pinMode(RELAY_LIGHT, OUTPUT); 
  pinMode(RELAY_LOCK, OUTPUT); 
  pinMode(RELAY_DOOR, OUTPUT); 
  pinMode(PIN_BUZZ, OUTPUT); 

  setupChronoDot();
  setupPrefs();
  setupLCD();  
  setupMenu();

  bootTone();
}

void loop() {
  tNow = millis();
  procLoopKeyPress();
  procLoopState();
}

void setupPrefs() {
  prefs.load();  
  
  hdlrKeySound.setValue(prefs.readBoolean(hdlrKeySound.getIdent(), KEY_SOUND_DEFAULT));
  hdlrBootSound.setValue(prefs.readBoolean(hdlrBootSound.getIdent(), BOOT_SOUND_DEFAULT));
  hdlrOtherSound.setValue(prefs.readBoolean(hdlrOtherSound.getIdent(), OTHER_SOUND_DEFAULT));
}

void setupMenu() {
  menu.setHead(&mOverrideOpen);
  mOverrideOpen.appendSibling(&mAutoClose);
  mAutoClose.appendSibling(&mLockTimes);
  mLockTimes.appendSibling(&mSettings);

  mLockTimes.setChild(&mLock1);
  mLock1.appendSibling(&mLock2);

  mSettings.setChild(&mOpenDuration);
  mOpenDuration.appendSibling(&mDate);
  mDate.appendSibling(&mBacklight);
  mBacklight.appendSibling(&mSounds);
  
  mSounds.setChild(&mKeySound);
  mKeySound.appendSibling(&mBootSound);
  mBootSound.appendSibling(&mOtherSound);
    
  currentHandler = NULL;
}

void bootTone() {
  if ( prefs.bootSound )
    tone(PIN_BUZZ, 4000, 50);
}

void keyTone() {
  if ( prefs.keySounds )
    tone(PIN_BUZZ, 3000, 10);
}

void confirmTone() {
  if ( prefs.otherSounds ) {
    tone(PIN_BUZZ, 1000, 100);
    delay(100);
    tone(PIN_BUZZ, 2000, 200);
  }
}

void cancelTone() {
  if ( prefs.otherSounds ) {
    tone(PIN_BUZZ, 1000, 100);
    delay(100);
    tone(PIN_BUZZ, 500, 200);
  }
}

void setupLCD() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(LCD_COLS, LCD_ROWS);

  pinMode(PIN_LCD_BL_PWR, OUTPUT); 
  enableLCD();
  
  displayMainHeader();
}

void setupChronoDot() {
  RTC.begin();

  if (! RTC.isrunning()) {
//#if DBG
//    Serial.println("RTC is NOT running!");
//#endif
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void procLoopState() {
  switch (state) {
    case STATE_IDLE:
      procLoopStateIdle();
      break;
  }
  
  if (state != STATE_IDLE && tNow - tLastKeyPress > INPUT_IDLE_TIMEOUT) {
//#if DBG
//    Serial.print("Idle timeout: "); Serial.println(tNow - tLastKeyPress);
//#endif
    setIdle();
  }
    
  //Things to do in every state
  readLight();    
}

void setIdle() {
  state = STATE_IDLE;
  menu.reset();
  lcd.clear();
  displayMainHeader();
}

void procLoopKeyPress() {
  int k = kpad.readKey();
  char c = kpad.getLastKeyChar();
    
  if ( k != KEY_NONE ) {
    keyTone();
//#if DBG
//    Serial.print("Menu key: code="); Serial.println(k);
//    Serial.print("          char="); Serial.println(c);
//#endif
    
    tLastKeyPress = tNow;
    switch (state) {
      case STATE_IDLE:
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
  if (k == KEY_STAR) {
    clearHandler(false);
  }
  else {
    if ( currentHandler != NULL && currentHandler->wantsControl() ) {
//#if DBG
//      Serial.println("Sending key to handler");
//#endif
      if (! currentHandler->procKeyPress(k, c)) {
        clearHandler(true);
      }
      else {
//#if DBG
//        Serial.println("Handler retained control");
//#endif
      }
    }
  }
}

void clearHandler(boolean confirmed) {
  lcd.clear();

  if ( confirmed ) {
//#if DBG
//    Serial.println("Handler relinquished control");
//#endif
    switch ( currentHandler->getValueType() ) {
      case TYPE_SHORT:
        prefs.writeShort(currentHandler->getIdent(), currentHandler->getValue());
      break;
      case TYPE_INT:
        prefs.writeInt(currentHandler->getIdent(), currentHandler->getValue());
      break;
      case TYPE_LONG:
        prefs.writeLong(currentHandler->getIdent(), currentHandler->getValue());
      break;
    }

    prefs.load();
    confirmTone();  
    currentHandler->displayConfirmation();
  }  
  else {
//#if DBG
//    Serial.println("Canceling handler");
//#endif
    cancelTone();  
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

void procLoopStateIdle() {
  /*
   * Actions to take in idle state.
   * We only read and act on the door switch in idle state.
   * So as to not go into auto close mode while the user 
   * is interacting with the device.
   */
  if (tNow - tLastIdleReading > IDLE_LOOP_UPDATE) {
    int hours=0, minutes=0, seconds=0;

    readTime(hours, minutes, seconds);
    displayTime(hours, minutes, seconds);

    float temp = readTemp(TEMP_F);
    displayTemp(TEMP_F, temp);

    tLastIdleReading = tNow;
    
    boolean doorOpenNow = isDoorOpen();
    if ( doorOpen != doorOpenNow ) {
      doorOpen = doorOpenNow;
      displayMainHeader();
    }
  }
}

boolean isDoorOpen() {
  /* door mag switch is active low */
  door.update();
  return !door.read();
}

void displayMainHeader() {
  lcd.home();
  if ( doorOpen ) 
    lcd.print("   Door Open    ");
  else
    lcd.print("Genie Steroids! ");
}

void h_setDateTime() {
//#if DBG
//  Serial.println("Set Date & Time");
//#endif
}

void displayMenu() {
  menu.display();
  state = STATE_MENU;
}

void toggleLockRelay(){
  digitalWrite(RELAY_LOCK, !digitalRead(RELAY_LOCK));
}

void activateLightRelay(){
  digitalWrite(RELAY_LIGHT, HIGH);
  delay(RELAY_DELAY);
  digitalWrite(RELAY_LIGHT, LOW);
}

void activateDoorRelay(){
  digitalWrite(RELAY_DOOR, HIGH);
  delay(RELAY_DELAY);
  digitalWrite(RELAY_DOOR, LOW);
}

void displayTime(const int hours, const int minutes, const int seconds) {
  if ( hours <= 12 )
    sprintf(sz_time, "%02d:%02d:%02d AM", hours, minutes, seconds);
  else
    sprintf(sz_time, "%02d:%02d:%02d PM", hours-12, minutes, seconds);
//#if DBG
//  Serial.print("Time: '"); Serial.println(sz_time);
//#endif
  
  lcd.setCursor(0, 1);
  lcd.print(sz_time);
}

void displayTemp(const int scale, const float temp) {
//#if DBG
//  Serial.print(temperatureF); Serial.println(" degress F");
//#endif
  sprintf(sz_temp, "%3d%c", int(temp), LCD_CHAR_DEGREES);
//#if DBG
//  Serial.print("TEMP: "); Serial.println(sz_temp);
//#endif
  
  lcd.setCursor(12, 1);
  lcd.print(sz_temp);
}

void readTime(int &hours, int &minutes, int &seconds) {
  DateTime now = RTC.now();
  hours = now.hour();
  minutes = now.minute();
  seconds = now.second();
}

void readLight() {
  //getting the voltage reading from the light sensor
  light = analogRead(SENSOR_LIGHT);  
//#if DBG
//  Serial.print("LIGHT: "); Serial.println(light);
//#endif
}

float readTemp(const int scale) {
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(SENSOR_TEMP);  
   
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

void enableLCD() {
    lcd.display();
    digitalWrite(PIN_LCD_BL_PWR, HIGH);
//#if DBG
//    Serial.println("LCD ON");
//#endif
}

void disableLCD() {
    lcd.noDisplay();
    digitalWrite(PIN_LCD_BL_PWR, LOW);
//#if DBG
//    Serial.println("LCD OFF");
//#endif
}

