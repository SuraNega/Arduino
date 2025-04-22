#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

RTC_DS3231 rtc;
char timeBuffer[32];
char dateBuffer[32];
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000; // Update interval in milliseconds (1 second)

// Create an instance of the SSD1306 display
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  oled.clearDisplay();  // Clear display
  oled.setTextSize(1);       // Text size
  oled.setTextColor(WHITE);  // Text color

  // Display initial content
  updateDisplay();
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if 1 second has passed
  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;
    
    // Update display content
    updateDisplay();
  }
}

void updateDisplay() {
  DateTime now = rtc.now();
  
  // Subtract 3 hours from the current time
  int adjustedHour = now.hour() - 3;
  if (adjustedHour < 0) {
    adjustedHour += 24; // Adjust the hour to be within the 0-23 range
  }
  
  // Format the adjusted time and date
  sprintf(timeBuffer, "%02d:%02d:%02d", 
          adjustedHour, 
          now.minute(), 
          now.second());  
  sprintf(dateBuffer, "%02d/%02d/%02d", 
          now.day(), 
          now.month(), 
          now.year());

  // Display the time and date on the OLED
  oled.clearDisplay();  // Clear display

  // Set cursor and print the time
  oled.setCursor(0, 0); // Top-left corner
  oled.println(timeBuffer); // Print the time string

  // Set cursor and print the date
  oled.setCursor(0, 20); // A bit lower for the date
  oled.println(dateBuffer); // Print the date string

  // Show the updated text
  oled.display();

  Serial.print(F("Date/Time: "));
  Serial.println(timeBuffer);
}
