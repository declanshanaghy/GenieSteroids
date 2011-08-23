#include <AnalogKeypad.h>
#include <Wire.h>
#include <LiquidCrystal.h>

/***************************
  ANALOG PIN DEFINIDTIONS
****************************/
#define PIN_KEYS    A0
#define PIN_TEMP    A1
#define PIN_LIGHT   A2
#define PIN_LOCK    A3

/***************************
  DIGITAL PIN DEFINIDTIONS
****************************/
#define PIN_LOCK     5
#define PIN_LIGHT    6
#define PIN_DOOR     7
#define PIN_BUZZ     8

#define PIN_LCD_BL_PWR   3
#define PIN_LCD_RS      13
#define PIN_LCD_EN       4
#define PIN_LCD_D4      12
#define PIN_LCD_D5      11
#define PIN_LCD_D6      10
#define PIN_LCD_D7       9

/***************************
  OTHER CONSTANTS
****************************/
#define LCD_CHAR_DEGREES 223

#define STATE_IDLE 0
#define RELAY_DELAY 100

/*****************************
  GLOBAL VARS
******************************/
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, 
                  PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
AnalogKeypad kpad = AnalogKeypad(PIN_KEYS, REPEAT_OFF, 10000, 1000);

int hours=0, minutes=0, seconds=0;
char sz_time[] = "00:00:00 AM";
float temperatureC, temperatureF;
char sz_temp[] = "999F";
int state = STATE_IDLE;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  kpad.init();
  
  pinMode(PIN_LIGHT, OUTPUT); 
  pinMode(PIN_LOCK, OUTPUT); 
  pinMode(PIN_DOOR, OUTPUT); 
  pinMode(PIN_BUZZ, OUTPUT); 
  pinMode(PIN_LCD_BL_PWR, OUTPUT); 

  setupLCD();  
  setupChronoDot();
  
  state = STATE_IDLE;
  //tone(PIN_BUZZ, 4000, 150);
}

void setupLCD() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Genie Steroids!");
  
  enableLCD();
}

void setupChronoDot() {
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  Wire.beginTransmission(0x68); // address DS3231
  Wire.send(0x0E); // select register
  Wire.send(0b00011100); // write register bitmap, bit 7 is /EOSC
  Wire.endTransmission();
}

long tReading = 0;
void loop() {
  long tNow = millis();

  procKeyPress();
  
  if (tNow - tReading > 500) {
    readTime();
    readTemp();
  
    displayTime();
    displayTemp();
    
    tReading = tNow;
  }
}

void procKeyPress() {
  int k = kpad.readKey();
  switch (k) {
    case KEY_NONE:
      break;
    case KEY_STAR:
      Serial.println("*");
      break;
    case KEY_POUND:
      Serial.println("#");
      break;
    case KEY_0:
      Serial.println("0");
      break;
    case KEY_1:
      Serial.println("1");
      toggleLockRelay();
      break;
    case KEY_2:
      Serial.println("2");
      activateLightRelay();
      break;
    case KEY_3:
      activateDoorRelay();
      Serial.println("3");
      break;
    case KEY_4:
      Serial.println("4");
      break;
    case KEY_5:
      Serial.println("5");
      break;
    case KEY_6:
      Serial.println("6");
      break;
    case KEY_7:
      Serial.println("7");
      break;
    case KEY_8:
      Serial.println("8");
      break;
    case KEY_9:
      Serial.println("9");
      break;
  }
}

void toggleLockRelay(){
  digitalWrite(PIN_LOCK, !digitalRead(PIN_LOCK));
}

void activateLightRelay(){
  digitalWrite(PIN_LIGHT, HIGH);
  delay(RELAY_DELAY);
  digitalWrite(PIN_LIGHT, LOW);
}

void activateDoorRelay(){
  digitalWrite(PIN_DOOR, HIGH);
  delay(RELAY_DELAY);
  digitalWrite(PIN_DOOR, LOW);
}

void displayTime() {
  lcd.setCursor(0, 1);
  lcd.print(sz_time);
}

void displayTemp() {
  lcd.setCursor(12, 1);
  lcd.print(sz_temp);
}

void readTime() {
  // send request to receive data starting at register 0
  Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
  Wire.send(0); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(0x68, 3); // request three bytes (seconds, minutes, hours)
 
  while(Wire.available())
  { 
    seconds = Wire.receive(); // get seconds
    minutes = Wire.receive(); // get minutes
    hours = Wire.receive();   // get hours
 
    seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
    minutes = (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal
    hours = (((hours & 0b00110000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
    
    if ( hours <= 12 )
      sprintf(sz_time, "%02d:%02d:%02d AM", hours, minutes, seconds);
    else
      sprintf(sz_time, "%02d:%02d:%02d PM", hours-12, minutes, seconds);
    //Serial.print("Time: '"); Serial.print(sz_time); Serial.println("'");
  }
}

void readTemp() {
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(PIN_TEMP);  
   
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
   
  // print out the voltage
  //Serial.print(voltage); Serial.println(" volts");
   
  // now print out the temperature
  temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                 //to degrees ((volatge - 500mV) times 100)
  //Serial.print(temperatureC); Serial.println(" degress C");
   
  // now convert to Fahrenheight
  temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  //Serial.print(temperatureF); Serial.println(" degress F");
  sprintf(sz_temp, "%3d%c", int(temperatureF), LCD_CHAR_DEGREES);
  //Serial.print("TEMP: "); Serial.print(sz_temp);
}

void enableLCD() {
    lcd.display();
    digitalWrite(PIN_LCD_BL_PWR, HIGH);
    Serial.println("LCD ON");
}

void disableLCD() {
    lcd.noDisplay();
    digitalWrite(PIN_LCD_BL_PWR, LOW);
    Serial.println("LCD OFF");
}
