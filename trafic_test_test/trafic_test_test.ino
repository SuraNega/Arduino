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
}

void loop() {
    // Measure distances
    distance1 = getDistance(trigPin1, echoPin1);
    distance2 = getDistance(trigPin2, echoPin2);

    // Check for short distance commands (must be 15 cm or less)
    bool currentShortDistance1 = distance1 <= 15;
    bool currentShortDistance2 = distance2 <= 15;

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
