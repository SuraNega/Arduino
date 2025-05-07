#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Speed Sensor Pins
const int sen1 = A1; // Sensor 1 pin
const int sen2 = A15; // Sensor 2 pin
const int trigPin1 = 13;
const int echoPin1 = 12;
const int trigPin2 = 14;
const int echoPin2 = 15;

// Traffic Light Pins
const int green1 = 7;
const int yellow1 = 6;
const int red1 = 5;
const int green2 = 4;
const int yellow2 = 3;
const int red2 = 2;

// Variables for speed measurement
float velocity;
long distance1, distance2;

// Traffic Light Variables
enum TrafficLightState {
    GREEN1,
    YELLOW1,
    GREEN2,
    YELLOW2
};

TrafficLightState currentState = GREEN1;
unsigned long stateStartTime = 0;
const unsigned long greenDuration = 4000; // 4 seconds for green lights
const unsigned long yellowDuration = 1000; // 1 second for yellow lights

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    pinMode(sen1, INPUT);
    pinMode(sen2, INPUT);
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    
    pinMode(green1, OUTPUT);
    pinMode(yellow1, OUTPUT);
    pinMode(red1, OUTPUT);
    pinMode(green2, OUTPUT);
    pinMode(yellow2, OUTPUT);
    pinMode(red2, OUTPUT);

    lcd.setCursor(0, 0);
    lcd.print(" Speed Detector ");
}

void loop() {
    measureSpeed();
    updateTrafficLights();
    delay(100); // Small delay to avoid rapid looping
}

void measureSpeed() {
    distance1 = getDistance(trigPin1, echoPin1);
    distance2 = getDistance(trigPin2, echoPin2);

    if (distance1 > 0 && distance2 > 0) {
        calculateVelocity();
    }
}

long getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    return (duration * 0.034) / 2; // Return distance in cm
}

void calculateVelocity() {
    // Wait for the first sensor to be triggered
    while (digitalRead(sen1) == HIGH);
    unsigned long t1 = micros();

    // Wait for the second sensor to be triggered
    while (digitalRead(sen2) == HIGH);
    unsigned long t2 = micros();

    unsigned long timeDiff = t2 - t1;

    if (timeDiff > 0) {
        velocity = (5.0 / (timeDiff / 1e6)) * 3.6; // Convert to Km/hr
        updateLCD(velocity);
    } else {
        Serial.println("Velocity: Calculation error (timeDiff is zero)");
    }
}

void updateLCD(float speed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Speed Detector ");
    lcd.setCursor(0, 1);
    lcd.print("Speed: ");
    lcd.print(speed, 2);
    lcd.print(" Km/hr  ");
}

void updateTrafficLights() {
    switch (currentState) {
        case GREEN1:
            if (millis() - stateStartTime >= greenDuration) {
                currentState = YELLOW1;
                stateStartTime = millis();
                digitalWrite(green1, LOW);
                digitalWrite(red2, LOW);
                digitalWrite(yellow1, HIGH);
                digitalWrite(yellow2, HIGH);
                Serial.println("Switching to YELLOW1");
            }
            break;

        case YELLOW1:
            if (millis() - stateStartTime >= yellowDuration) {
                currentState = GREEN2;
                stateStartTime = millis();
                digitalWrite(yellow1, LOW);
                digitalWrite(yellow2, LOW);
                digitalWrite(green2, HIGH);
                digitalWrite(red1, HIGH);
                Serial.println("Switching to GREEN2");
            }
            break;

        case GREEN2:
            if (millis() - stateStartTime >= greenDuration) {
                currentState = YELLOW2;
                stateStartTime = millis();
                digitalWrite(green2, LOW);
                digitalWrite(red1, LOW);
                digitalWrite(yellow1, HIGH);
                digitalWrite(yellow2, HIGH);
                Serial.println("Switching to YELLOW2");
            }
            break;

        case YELLOW2:
            if (millis() - stateStartTime >= yellowDuration) {
                currentState = GREEN1;
                stateStartTime = millis();
                digitalWrite(yellow1, LOW);
                digitalWrite(yellow2, LOW);
                digitalWrite(green1, HIGH);
                digitalWrite(red2, HIGH);
                Serial.println("Switching to GREEN1");
            }
            break;
    }
}
