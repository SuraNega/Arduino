#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 20 chars and 4 lines display
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int sen1 = A1; // Sensor 1 pin
const int sen2 = A15; // Sensor 2 pin
unsigned long t1 = 0;
unsigned long t2 = 0; 
float velocity;

// Pin Definitions
const int green1 = 7;
const int yellow1 = 6;
const int red1 = 5;
const int green2 = 4;
const int yellow2 = 3;
const int red2 = 2;

const int trigPin1 = 13;
const int echoPin1 = 12;
const int trigPin2 = 14;
const int echoPin2 = 15;

long distance1, distance2;

enum TrafficLightState {
    GREEN1,
    YELLOW1,
    GREEN2,
    YELLOW2,
    PRIORITY1,
    PRIORITY2
};

TrafficLightState currentState = GREEN1;
unsigned long stateStartTime = 0;
unsigned long priorityStartTime = 0;
bool priorityActive = false; // Flag to indicate if priority is active
int priorityDirection = 0; // 1 for direction 1, 2 for direction 2
unsigned long shortDistanceStartTime1 = 0; // Start time for direction 1
unsigned long shortDistanceStartTime2 = 0; // Start time for direction 2
bool shortDistanceDetected1 = false; // Short distance flag for direction 1
bool shortDistanceDetected2 = false; // Short distance flag for direction 2

const unsigned long greenDuration = 4000; // 4 seconds for green lights
const unsigned long yellowDuration = 1000; // 1 second for yellow lights
const unsigned long shortDistanceDelay = 3000; // 3 seconds for short distance

long getDistance(int trigPin, int echoPin);
void calculateVelocity();

void setup() {
    pinMode(green1, OUTPUT);
    pinMode(yellow1, OUTPUT);
    pinMode(red1, OUTPUT);
    pinMode(green2, OUTPUT);
    pinMode(yellow2, OUTPUT);
    pinMode(red2, OUTPUT);

    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);

    Serial.begin(9600);
    stateStartTime = millis(); // Initialize timing

    lcd.init();         // Initialize the LCD
    lcd.backlight();    // Turn on the backlight
    pinMode(sen1, INPUT);
    pinMode(sen2, INPUT);
  
    // Initial message on the LCD
    lcd.setCursor(0, 0);
    lcd.print(" Speed Detector ");
}

void loop() {
    // Measure distances
    distance1 = getDistance(trigPin1, echoPin1);
    distance2 = getDistance(trigPin2, echoPin2);

    calculateVelocity(); // Call the velocity calculation

    // Check for short distance commands
    bool currentShortDistance1 = distance1 <= 20;
    bool currentShortDistance2 = distance2 <= 20;

    // Update short distance status for direction 1
    if (currentShortDistance1) {
        if (!shortDistanceDetected1) {
            shortDistanceStartTime1 = millis(); // Start timing for short distance
        }
        shortDistanceDetected1 = true; // Mark short distance as detected
    } else {
        shortDistanceDetected1 = false; // Reset if no short distance
    }

    // Update short distance status for direction 2
    if (currentShortDistance2) {
        if (!shortDistanceDetected2) {
            shortDistanceStartTime2 = millis(); // Start timing for short distance
        }
        shortDistanceDetected2 = true; // Mark short distance as detected
    } else {
        shortDistanceDetected2 = false; // Reset if no short distance
    }

    // Activate priority if a consistent short distance is detected for 3 seconds
    if (!priorityActive) {
        if (shortDistanceDetected1 && (millis() - shortDistanceStartTime1 >= shortDistanceDelay)) {
            priorityDirection = 1;
            priorityStartTime = millis(); // Start timing for direction 1
            priorityActive = true; // Activate priority
            Serial.println("Priority to GREEN1 due to sustained short distance.");
        } else if (shortDistanceDetected2 && (millis() - shortDistanceStartTime2 >= shortDistanceDelay)) {
            priorityDirection = 2;
            priorityStartTime = millis(); // Start timing for direction 2
            priorityActive = true; // Activate priority
            Serial.println("Priority to GREEN2 due to sustained short distance.");
        }
    }

    // Handle priority logic
    if (priorityActive) {
        if (millis() - priorityStartTime < shortDistanceDelay) {
            // Maintain the priority state
            if (priorityDirection == 1) {
                digitalWrite(green1, HIGH);
                digitalWrite(red2, HIGH);
                digitalWrite(green2, LOW);
                digitalWrite(red1, LOW);
            } else if (priorityDirection == 2) {
                digitalWrite(green2, HIGH);
                digitalWrite(red1, HIGH);
                digitalWrite(green1, LOW);
                digitalWrite(red2, LOW);
            }
            return; // Exit loop to avoid processing normal traffic light sequence
        } else {
            // Reset priority after 3 seconds
            priorityActive = false;
            priorityDirection = 0; // Reset direction
        }
    }

    // Normal traffic light sequence if no priority active
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

        default:
            break;
    }

    // Print distances for debugging
    Serial.print("Distance 1: ");
    Serial.println(distance1);
    Serial.print("Distance 2: ");
    Serial.println(distance2);
}

long getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    long distance = duration * 0.034 / 2;
    return distance;
}

void calculateVelocity() {
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
