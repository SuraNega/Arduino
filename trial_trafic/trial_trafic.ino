#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>

#define TRIG_PIN_OLD 9
#define ECHO_PIN_OLD 8
#define TRIG_PIN_NEW 2
#define ECHO_PIN_NEW 3
#define MAX_DISTANCE 400  // Maximum distance in cm
#define TIME_INTERVAL 1000  // Time interval between measurements in milliseconds
#define LED_PIN 7  // Pin for the LED
#define SPEED_THRESHOLD 20  // Speed threshold in cm/s

// Initialize the LCD (20 columns and 4 rows)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Initialize the HC-SR04 sensors
NewPing sonarOld(TRIG_PIN_OLD, ECHO_PIN_OLD, MAX_DISTANCE);
NewPing sonarNew(TRIG_PIN_NEW, ECHO_PIN_NEW, MAX_DISTANCE);

unsigned long previousMillis = 0;
float previousDistanceOld = 0;
float previousDistanceNew = 0;

void setup() {
  Serial.begin(9600);
  
  // Initialize the LCD and sensor
  lcd.init();
  lcd.backlight();
  
  // Set up the LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Ensure the LED is off initially

  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Speed Measurement");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  delay(80);  // Short delay to display the message
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Measure distances from both sensors
  float currentDistanceOld = sonarOld.ping_cm();  // Get the current distance from the old sensor
  float currentDistanceNew = sonarNew.ping_cm();  // Get the current distance from the new sensor

  // Print distances
  Serial.print("Old Sensor Distance: ");
  Serial.println(currentDistanceOld);
  Serial.print("New Sensor Distance: ");
  Serial.println(currentDistanceNew);

  // Ensure the time interval has passed before calculating speed
  if (currentMillis - previousMillis >= TIME_INTERVAL) {
    previousMillis = currentMillis;

    // Calculate the speed (cm/s) for both sensors
    float speedOld = (currentDistanceOld - previousDistanceOld) / (TIME_INTERVAL / 1000.0);  // Speed in cm/s
    float speedNew = (currentDistanceNew - previousDistanceNew) / (TIME_INTERVAL / 1000.0);  // Speed in cm/s
    
    // Update the previous distances
    previousDistanceOld = currentDistanceOld;
    previousDistanceNew = currentDistanceNew;

    // Print the distances and speeds on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Old Distance: ");
    lcd.print(currentDistanceOld);
    lcd.print(" cm");
    
    lcd.setCursor(0, 1);
    lcd.print("Old Speed: ");
    lcd.print(speedOld, 2);  // Print the speed with two decimal places
    lcd.print(" cm/s");

    lcd.setCursor(0, 2);
    lcd.print("New Distance: ");
    lcd.print(currentDistanceNew);
    lcd.print(" cm");
    
    lcd.setCursor(0, 3);
    lcd.print("New Speed: ");
    lcd.print(speedNew, 2);  // Print the speed with two decimal places
    lcd.print(" cm/s");

    // Print the speeds to the Serial Monitor (optional)
    Serial.print("Old Speed: ");
    Serial.print(speedOld);
    Serial.println(" cm/s");
    Serial.print("New Speed: ");
    Serial.print(speedNew);
    Serial.println(" cm/s");

    // Check if the speed exceeds the threshold for either sensor
    if (speedOld > SPEED_THRESHOLD || speedNew > SPEED_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH);  // Turn on the LED if either sensor detects speed above the threshold
    } else {
      digitalWrite(LED_PIN, LOW);  // Turn off the LED if neither sensor detects speed above the threshold
    }
  }

  // Add a small delay to avoid flooding the Serial Monitor
  delay(80);
}
