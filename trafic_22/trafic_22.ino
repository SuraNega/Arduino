// Pin Definitions
const int green1 = 7;
const int yellow1 = 6;
const int red1 = 5;
const int green2 = 4;
const int yellow2 = 3;
const int red2 = 2;

const int trigPin1 = 10;
const int echoPin1 = 11;
const int trigPin2 = 13;
const int echoPin2 = 12;

long distance1, distance2;

enum TrafficLightState {
    IDLE,
    GREEN1,
    YELLOW1,
    GREEN2,
    YELLOW2,
    SHORT_DISTANCE1,
    SHORT_DISTANCE2
};

TrafficLightState currentState = IDLE;
unsigned long stateStartTime = 0;
bool shortDistanceActive1 = false;
bool shortDistanceActive2 = false;

const unsigned long greenDuration = 4000; // 4 seconds for green lights
const unsigned long yellowDuration = 1000; // 1 second for yellow lights
const unsigned long shortDistanceDelay = 3000; // 3 seconds for short distance

long getDistance(int trigPin, int echoPin);

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
}

void loop() {
    // Measure distances
    distance1 = getDistance(trigPin1, echoPin1);
    distance2 = getDistance(trigPin2, echoPin2);

    // Check for short distance commands
    if (!shortDistanceActive1 && (distance1 <= 10)) {
        shortDistanceActive1 = true; // Activate the first short distance command
        Serial.println("First short distance detected.");
    }

    if (!shortDistanceActive2 && (distance2 <= 10)) {
        shortDistanceActive2 = true; // Activate the second short distance command
        Serial.println("Second short distance detected.");
    }

    // Handle short distance for first side
    if (shortDistanceActive1 && !shortDistanceActive2) {
        currentState = SHORT_DISTANCE1;
        digitalWrite(green1, HIGH);
        digitalWrite(red2, HIGH);
        stateStartTime = millis();
        Serial.println("Priority to GREEN1 due to first short distance.");
    }

    // Handle short distance for second side only if first is inactive
    else if (shortDistanceActive2 && !shortDistanceActive1) {
        currentState = SHORT_DISTANCE2;
        digitalWrite(green2, HIGH);
        digitalWrite(red1, HIGH);
        stateStartTime = millis();
        Serial.println("Priority to GREEN2 due to second short distance.");
    }

    // Control traffic light sequence
    switch (currentState) {
        case IDLE:
            currentState = GREEN1;
            stateStartTime = millis();
            digitalWrite(green1, HIGH);
            digitalWrite(red2, HIGH);
            Serial.println("Starting GREEN1");
            break;

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

        case SHORT_DISTANCE1:
            if (millis() - stateStartTime >= shortDistanceDelay) {
                shortDistanceActive1 = false; // Reset first short distance
                currentState = GREEN2; // Switch to GREEN2 afterwards
                stateStartTime = millis();
                digitalWrite(green1, LOW);
                digitalWrite(red2, LOW);
                Serial.println("First short distance complete, switching to GREEN2.");
            }
            break;

        case SHORT_DISTANCE2:
            if (millis() - stateStartTime >= shortDistanceDelay) {
                shortDistanceActive2 = false; // Reset second short distance
                currentState = GREEN1; // Switch to GREEN1 afterwards
                stateStartTime = millis();
                digitalWrite(green2, LOW);
                digitalWrite(red1, LOW);
                Serial.println("Second short distance complete, switching to GREEN1.");
            }
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
