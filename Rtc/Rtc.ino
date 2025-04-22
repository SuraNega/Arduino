#include <DS3231.h>
#include <LiquidCrystal.h> // includes the LiquidCrystal Library 

DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(1, 2, 4, 5, 6, 7); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 

void setup() { 
 rtc.begin(); // Initialize the rtc object
 lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
}

void loop() { 
 lcd.setCursor(0,0);
 lcd.print("Time:  ");
 lcd.print(rtc.getTimeStr());
 
 lcd.setCursor(0,1);
 lcd.print("Date: ");
 lcd.print(rtc.getDateStr());
 
 delay(1000); 
}
