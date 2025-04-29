#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 20 chars and 4 lines display
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int sen1 = A1; // Sensor 1 pin
const int sen2 = A15; // Sensor 2 pin
unsigned long t1 = 0;
unsigned long t2 = 0; 
float velocity;

void setup() {
  lcd.init();         // Initialize the LCD
  lcd.backlight();    // Turn on the backlight
  pinMode(sen1, INPUT);
  pinMode(sen2, INPUT);
  Serial.begin(9600); // Initialize Serial communication
  
  // Initial message on the LCD
  lcd.setCursor(0, 0);
  lcd.print(" Speed Detector ");
}

void loop() {
  // Wait for the first sensor to be triggered
  while (digitalRead(sen1) == HIGH);
  t1 = micros(); // Start time in microseconds
  
  // Wait for the second sensor to be triggered
  while (digitalRead(sen2) == HIGH);
  t2 = micros(); // End time in microseconds

  // Calculate time difference
  unsigned long timeDiff = t2 - t1;

  if (timeDiff > 0) { // Check to prevent division by zero
    // Calculate velocity (5 meters is the distance between sensors)
    velocity = (5.0 / (timeDiff / 1e6)) * 3.6; // Convert to Km/hr

    // Check for realistic velocity values
    if (velocity < 0 || velocity > 200) { // Adjust the max value based on expected speed
      Serial.println("Velocity: Calculation error (unrealistic value)");
    } else {
      // Print velocity on the Serial Monitor
      Serial.print("Velocity: ");
      Serial.print(velocity);
      Serial.println(" Km/hr");

      // Clear the LCD and display velocity
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Speed Detector ");
      lcd.setCursor(0, 1);
      lcd.print("Speed: ");
      lcd.print(velocity, 2); // Print with 2 decimal places
      lcd.print(" Km/hr  "); // Ensure there's space to clear previous values
    }
  } else {
    Serial.println("Velocity: Calculation error (timeDiff is zero)");
  }

  // Short delay to allow for next measurement
  delay(500); // Adjust as needed
}
