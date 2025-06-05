#include <RTClib.h>
#include <Wire.h>
RTC_DS3231 rtc;
char t[32];

unsigned long lastTime = 0; // Variable to store the last printed time

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
}

void loop() {
  // Get the current time from RTC
  DateTime now = rtc.now();

  // Check if 5 seconds have passed since the last time printed
  if (millis() - lastTime >= 5000) {
    lastTime = millis();  // Update lastTime for next check

    // Create a new DateTime object with 5 seconds added (simulated)
    DateTime displayedTime = now + TimeSpan(0, 0, 5);

    // Format the displayed time string
    sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", displayedTime.hour(), displayedTime.minute(), displayedTime.second(), displayedTime.day(), displayedTime.month(), displayedTime.year());

    Serial.print(F("Date/Time: "));
    Serial.println(t);
  }

  // Delay for any additional processing or tasks (optional)
  delay(1000); // You can adjust the delay as needed
}
