#include <LiquidCrystal.h>


#define PIN_BUZZ 8

#define PIN_LCD_BL_PWR 3
#define PIN_LCD_RS 13
#define PIN_LCD_EN 4
#define PIN_LCD_D4 12
#define PIN_LCD_D5 11
#define PIN_LCD_D6 10
#define PIN_LCD_D7 9
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, 
                  PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUZZ, OUTPUT); 
  pinMode(PIN_LCD_BL_PWR, OUTPUT); 
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  
  tone(PIN_BUZZ, 4000, 150);
}

void loop() {
  flash();  
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
  
  delay(1000);
}

void flash() {
//  digitalWrite(8, digitalRead(8)==LOW);
//  tone(8, 100, 10);

  if (digitalRead(PIN_LCD_BL_PWR) == LOW) {
    lcd.display();
    digitalWrite(PIN_LCD_BL_PWR, HIGH);
    Serial.println("ON");
  }
  else {
    lcd.noDisplay();
    digitalWrite(PIN_LCD_BL_PWR, LOW);
    Serial.println("OFF");
  }
}

