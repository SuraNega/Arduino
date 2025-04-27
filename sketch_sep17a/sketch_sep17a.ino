const int green1 = 20;
const int yellow1 = 21;
const int red1 = 19;
const int green2 = 4;
const int yellow2 = 3;
const int red2 = 2;

const int trigPin1 = 10;
const int echoPin1 = 11;
const int trigPin2 =13;
const int echoPin2 = 12;

long duration1, distance1;
long duration2, distance2;

unsigned long startTime1 = 0;
unsigned long startTime2 = 0;
unsigned long startTime3 = 0;
bool sensor1Triggered = false;
bool sensor2Triggered = false;
bool sensor3Triggered = false;

int n=0;
long getDistance(int trigPin, int echoPin);
void runSequence1();
void runSequence2();

void setup() {
  // Initialize LED pins as outputs
  pinMode(green1, OUTPUT);
  pinMode(yellow1, OUTPUT);
  pinMode(red1, OUTPUT);
  pinMode(green2, OUTPUT);
  pinMode(yellow2, OUTPUT);
  pinMode(red2, OUTPUT);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Measure distance from sensor 1
  distance1 = getDistance(trigPin1, echoPin1);

  // Measure distance from sensor 2
  distance2 = getDistance(trigPin2, echoPin2);

  // Yellow lights for 3 seconds
  digitalWrite(yellow1, HIGH);
  digitalWrite(yellow2, HIGH);
  delay(6000);

  // Green1 and Red2 for 7 seconds
  digitalWrite(yellow1, LOW);
  digitalWrite(yellow2, LOW);

  // Check conditions for sensor 1
  if (distance1 <= 30) {
    if (!sensor1Triggered) {
      startTime1 = millis();  // Start the timer
      sensor1Triggered = true;
    }
    // Check if 7 seconds have passed
    if (millis() - startTime1 >= 6000) {
      runSequence1();
      sensor1Triggered = false;  // Reset the trigger after running the sequence
    }
  } else {
    sensor1Triggered = false;  // Reset if distance is not within range
  }

  // Check conditions for sensor 2
  if (distance2 <= 50) {
    if (!sensor2Triggered) {
      startTime2 = millis();  // Start the timer
      sensor2Triggered = true;
    }
    // Check if 7 seconds have passed
    if (millis() - startTime2 >= 6000) {
      runSequence2();
      sensor2Triggered = false;  // Reset the trigger after running the sequence
    }
  } else {
    sensor2Triggered = false;  // Reset if distance is not within range
  }

  // Check conditions for sensor 3
  if (distance2 > 50 && distance1 > 30 || distance1 <= 30 && distance2 <= 50) {
    if (!sensor3Triggered) {
      startTime3 = millis();  // Start the timer
      sensor3Triggered = true;
    }
    // Check if 7 seconds have passed
    if (millis() - startTime3 >= 6000) {
        runSequence1();
        sensor3Triggered = false;
      }
    }
  else {
    sensor3Triggered = false;  // Reset if distance is not within range
  }

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

void runSequence1() {
  digitalWrite(green1, HIGH);
  digitalWrite(red2, HIGH);
  delay(60000);

  // Yellow lights for 3 seconds
  digitalWrite(green1, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(yellow1, HIGH);
  digitalWrite(yellow2, HIGH);
  delay(6000);

  // Green2 and Red1 for 7 seconds
  digitalWrite(yellow1, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(green2, HIGH);
  digitalWrite(red1, HIGH);
  delay(60000);

  // Turn off all lights
  digitalWrite(green2, LOW);
  digitalWrite(red1, LOW);
  sensor3Triggered = false;
  loop();
}

void runSequence2() {
  digitalWrite(green2, HIGH);
  digitalWrite(red1, HIGH);
  delay(60000);

  // Yellow lights for 3 seconds
  digitalWrite(green2, LOW);
  digitalWrite(red1, LOW);
  digitalWrite(yellow1, HIGH);
  digitalWrite(yellow2, HIGH);
  delay(6000);

  // Green1 and Red2 for 7 seconds
  digitalWrite(yellow1, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(green1, HIGH);
  digitalWrite(red2, HIGH);
  delay(60000);
}
