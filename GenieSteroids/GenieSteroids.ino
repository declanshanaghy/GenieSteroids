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

#define DBG 1

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

#define INTERVAL_LIGHT_UPDATE 100
#define N_LIGHT_VALUES 10

#define RELAY_DELAY 250

// How long between keypresses until we go back to idle state
#define INPUT_IDLE_TIMEOUT 3000

// How long between keypresses until the LCD is disabled
#define INPUT_IDLE_LCD_OFF 5000

/*****************************
  GLOBAL VARS
******************************/
const DateTime COMPILE_TIME = DateTime(__DATE__, __TIME__);

boolean lcdOn = false;
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D7, 
                  PIN_LCD_D6, PIN_LCD_D5, PIN_LCD_D4);
AnalogKeypad kpad = AnalogKeypad(SENSOR_KEYS, REPEAT_OFF, 10000, 1000);
Chronodot chronodot;
GeniePrefs prefs;
HomeScreen homeScreen(lcd, chronodot, SENSOR_TEMP);
DoorController doorCtrl(&doorControllerCallback, prefs, PIN_DOOR_SENS, RELAY_DOOR, RELAY_DELAY);

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
DateHandler hdlrDate(CFG_UNUSED);
TimeHandler hdlrTime(CFG_UNUSED);
IntervalHandler hdlrOpenDuration(CFG_OPEN_DURATION);

LcdMenu menu(&lcd, LCD_COLS, LCD_ROWS);
LcdMenuEntry mOpenDuration(MENU_1, "Close Timer", &hdlrOpenDuration);
LcdMenuEntry mDate(MENU_2, "Set Date", &hdlrDate);
LcdMenuEntry mTime(MENU_3, "Set Time", &hdlrTime);
LcdMenuEntry mSounds(MENU_4, "Sounds", NULL);

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
  Serial.begin(115200);

  state = STATE_IDLE;
  
  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_LOCK, OUTPUT); 
  pinMode(PIN_BUZZ, OUTPUT); 

  kpad.init(); 
  setupChronoDot();
  setupPrefs();
  setupLCD();  
  setupMenu();

  toneBoot();
}

void loop() {
//#if DBG
//  Serial.print("freeRam: ");
//  Serial.println(freeRam());
//#endif
  doorCtrl.loop();
  procLoopKeyPress();
  procLoopState();
}

void doorControllerCallback(short msg, unsigned long countdown) {  
  tLastActivity = millis();
  
  if ( !lcdOn )
    enableLCD();
  
  switch ( msg ) {
  case MSG_DOOR_OPEN:
    if ( state == STATE_IDLE ) {      
      lcd.home();
      lcd.print("      OPEN      ");
    }
    break;
  case MSG_DOOR_CLOSED:
    homeScreen.display();
    break;
  case MSG_CLOSE_DOOR_COUNTDOWN:
    if ( state == STATE_IDLE ) {
      lcd.home();
      lcd.print("Close:          ");
      lcd.setCursor(7, 0);
      if ( countdown > 60 ) {
        lcd.print(countdown/60);
        lcd.print("m");
      }
      else {
        lcd.print(countdown);
        lcd.print("s");
      }
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
    toneCancel(true);
    break;
  }
}

void setupPrefs() {
  prefs.load();
  hdlrKeySound.setValue(prefs.readBoolean(hdlrKeySound.getIdent(), KEY_SOUND_DEFAULT));
  hdlrBootSound.setValue(prefs.readBoolean(hdlrBootSound.getIdent(), BOOT_SOUND_DEFAULT));
  hdlrOtherSound.setValue(prefs.readBoolean(hdlrOtherSound.getIdent(), OTHER_SOUND_DEFAULT));
  hdlrOpenDuration.setValue(prefs.readInt(hdlrOpenDuration.getIdent(), OPEN_DURATION_DEFAULT));
}

void setupMenu() {
  menu.setHead(&mOpenDuration);
  mOpenDuration.appendSibling(&mDate);
  mDate.appendSibling(&mTime);
  mTime.appendSibling(&mSounds);
  
  mSounds.setChild(&mKeySound);
  mKeySound.appendSibling(&mBootSound);
  mBootSound.appendSibling(&mOtherSound);
    
  currentHandler = NULL;
}

void toneBoot() {
  if ( prefs.bootSound )
    tone(PIN_BUZZ, 4000, 50);
}

void toneKey() {
  if ( prefs.keySounds )
    tone(PIN_BUZZ, 3000, 10);
}

void toneConfirm() {
  if ( prefs.otherSounds ) {
    tone(PIN_BUZZ, 1000, 100);
    delay(100);
    tone(PIN_BUZZ, 2000, 200);
  }
}

void toneCancel(boolean force) {
  if ( force || prefs.otherSounds ) {
    tone(PIN_BUZZ, 1000, 100);
    delay(100);
    tone(PIN_BUZZ, 500, 200);
  }
}

void toneInvalid() {
  tone(PIN_BUZZ, 75, 50);
}

void setupLCD() {  
  // set up the LCD's number of columns and rows: 
  lcd.begin(LCD_COLS, LCD_ROWS);

  pinMode(PIN_LCD_BL_PWR, OUTPUT); 
  enableLCD();
  
  homeScreen.display();
}

void setupChronoDot() {
  chronodot.begin();

//  if (! chronodot.isrunning()) {
//#if DBG
//    Serial.println("chronodot is NOT running!");
//#endif
    // following line sets the chronodot to the date & time this sketch was compiled
    chronodot.adjust(COMPILE_TIME);
//  }
}

void procLoopState() {
  unsigned long tNow = millis();

  switch (state) {
    case STATE_IDLE:
      homeScreen.loop();
      break;
  }
  
  if (state != STATE_IDLE && tNow > tLastActivity + INPUT_IDLE_TIMEOUT) {
//#if DBG
//    Serial.print("Idle timeout: "); Serial.println(tNow - tLastKeyPress);
//#endif
    setIdle();
  }
    
  if (state == STATE_IDLE && tNow > tLastActivity + INPUT_IDLE_LCD_OFF) {
//#if DBG
//    Serial.print("Idle timeout (lcd off): "); Serial.println(tNow - tLastKeyPress);
//#endif
    disableLCD();    
  }
  else {
    //Things to do in every state
    adjustBacklight();
  }
}

void setIdle() {
  state = STATE_IDLE;
  menu.reset();
  homeScreen.display();
}

void procLoopKeyPress() {
  unsigned long tNow = millis();
  
  int k = kpad.readKey();
  char c = kpad.getLastKeyChar();
    
  if ( k != KEY_NONE ) {
    if ( !lcdOn )
      enableLCD();
      
    toneKey();
//#if DBG
//    Serial.print("Menu key: code="); Serial.println(k);
//    Serial.print("          char="); Serial.println(c);
//#endif
    
    tLastActivity = tNow;
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
        prefs.writeShort(currentHandler->getIdent(), currentHandler->getValue());
        break;
      case TYPE_INT:
        prefs.writeInt(currentHandler->getIdent(), currentHandler->getValue());
        break;
      case TYPE_LONG:
        prefs.writeLong(currentHandler->getIdent(), currentHandler->getValue());
        break;
//      default:
//#if DBG
//    Serial.println("*** No value saved ***");
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
    toneCancel(false);  
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

void toggleLockRelay(){
  digitalWrite(RELAY_LOCK, !digitalRead(RELAY_LOCK));
}

void activateLightRelay(){
  digitalWrite(RELAY_LIGHT, HIGH);
  delay(RELAY_DELAY);
  digitalWrite(RELAY_LIGHT, LOW);
}

void adjustBacklight() {
  unsigned long tNow = millis();
  
  if ( lcdOn && (tLastLightUpdate == 0 || tNow > tLastLightUpdate + INTERVAL_LIGHT_UPDATE) ) {
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
  lcdOn = true;
  lcd.display();
  analogWrite(PIN_LCD_BL_PWR, light_avg);
}

void disableLCD() {
  lcdOn = false;
  lcd.noDisplay();
  digitalWrite(PIN_LCD_BL_PWR, LOW);
}

