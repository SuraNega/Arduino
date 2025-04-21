#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_I2C_ADDR 0x3D // or 0x3C
#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
#define OLED_RST_PIN -1       // Reset pin (-1 if not available)

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST_PIN);

void setup() {
  // Initialize the display
  display.begin(SSD1306_I2C_ADDRESS, SCREEN_I2C_ADDR);
  display.clearDisplay();
  
  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Draw the mute icon (simple representation)
  drawMuteIcon();
  
  // Display the buffer
  display.display();
}

void loop() {
  // Nothing to do here
}

void drawMuteIcon() {
  // Draw a simple mute icon (crossed speaker)
  
  // Draw speaker body
  display.fillRect(40, 20, 30, 20, WHITE); // Speaker body
  display.fillTriangle(40, 20, 40, 40, 20, 30, WHITE); // Speaker cone
  
  // Draw cross (mute sign)
  display.drawLine(20, 20, 60, 40, WHITE); // Diagonal line
  display.drawLine(20, 40, 60, 20, WHITE); // Diagonal line
}
