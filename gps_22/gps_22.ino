#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial gpsSerial(2, 1); // RX, TX for GPS
TinyGPSPlus gps;

// Define target location (latitude, longitude)
const float targetLatitude = 8.764778; // Target Latitude
const float targetLongitude = 38.999417; // Target Longitude

// Servo objects
Servo leftServo;
Servo rightServo;

// Define servo pins
#define LEFT_SERVO_PIN 8
#define RIGHT_SERVO_PIN 7

// Servo speed/pulse width
#define STOP 1500   // Neutral position (stop)
#define MAX_SPEED 2000 // Max speed for forward movement
#define MIN_SPEED 1000 // Max speed for backward movement

// Variable to store the last distance to the target
float lastDistance = 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  
  // Attach servos
  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);
  
  // Initialize servos to stop position
  stopRobot();
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
    
    if (gps.location.isUpdated()) {
      float currentLatitude = gps.location.lat();
      float currentLongitude = gps.location.lng();

      // Print current location
      Serial.print("Current Location: ");
      Serial.print("Latitude: ");
      Serial.print(currentLatitude, 6);
      Serial.print(", Longitude: ");
      Serial.println(currentLongitude, 6);
      
      float distance = calculateDistance(currentLatitude, currentLongitude, targetLatitude, targetLongitude);
      float bearing = calculateBearing(currentLatitude, currentLongitude, targetLatitude, targetLongitude);
      
      Serial.print("Distance to target: ");
      Serial.print(distance);
      Serial.println(" meters");
      
      Serial.print("Bearing to target: ");
      Serial.println(bearing);

      // Check if the distance is increasing or decreasing
      if (lastDistance != 0) {
        if (distance > lastDistance) {
          // Distance is increasing, stop and change direction
          stopRobot();
          changeDirection(currentLatitude, currentLongitude);
        } else {
          // Distance is decreasing, move towards the target
          moveRobot(bearing, false); // Move forward
        }
      }
      
      // Update lastDistance
      lastDistance = distance;

      // Stop when you reach the target
      if (distance <= 5.0) {
        stopRobot(); 
      }
    }
  }
}

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  const float R = 6371000; // Radius of the Earth in meters
  float phi1 = lat1 * (PI / 180);
  float phi2 = lat2 * (PI / 180);
  float deltaPhi = (lat2 - lat1) * (PI / 180);
  float deltaLambda = (lon2 - lon1) * (PI / 180);
  
  float a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
            cos(phi1) * cos(phi2) *
            sin(deltaLambda / 2) * sin(deltaLambda / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  
  return R * c; // Distance in meters
}

float calculateBearing(float lat1, float lon1, float lat2, float lon2) {
  float deltaLambda = (lon2 - lon1) * (PI / 180);
  lat1 = lat1 * (PI / 180);
  lat2 = lat2 * (PI / 180);

  float x = sin(deltaLambda) * cos(lat2);
  float y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(deltaLambda);
  
  float initialBearing = atan2(x, y);
  initialBearing = fmod((initialBearing * (180 / PI) + 360), 360); // Convert to degrees
  return initialBearing; // Bearing in degrees
}

void moveRobot(float bearing, bool adjustHeading) {
  // Placeholder for the robot's current heading
  float currentHeading = 0; // Replace with actual heading data

  // Calculate the bearing difference
  float bearingDifference = bearing - currentHeading;

  // Normalize bearingDifference to range [-180, 180]
  if (bearingDifference > 180) bearingDifference -= 360;
  if (bearingDifference < -180) bearingDifference += 360;

  // Speeds for the servos
  int leftSpeed = STOP;
  int rightSpeed = STOP;

  // Define a smaller threshold for turning precision
  const float turnThreshold = 2.0; // Degrees for turning

  // If already moving towards the target
  if (abs(bearingDifference) <= turnThreshold) {
    leftSpeed = MAX_SPEED;
    rightSpeed = MAX_SPEED; // Move forward confidently
  } else {
    // If not aligned, adjust heading
    if (bearingDifference > 0) { // Turn right
      leftSpeed = MIN_SPEED;
      rightSpeed = MAX_SPEED;
    } else { // Turn left
      leftSpeed = MAX_SPEED;
      rightSpeed = MIN_SPEED;
    }
  }

  // Set servo speeds
  leftServo.writeMicroseconds(leftSpeed);
  rightServo.writeMicroseconds(rightSpeed);
}

void changeDirection(float currentLatitude, float currentLongitude) {
  // Calculate a new random direction to turn towards
  float randomAngle = random(-45, 45); // Random angle between -45 and 45 degrees
  float newBearing = calculateBearing(currentLatitude, currentLongitude, targetLatitude, targetLongitude) + randomAngle;

  // Normalize the new bearing
  if (newBearing < 0) newBearing += 360;
  if (newBearing >= 360) newBearing -= 360;

  // Turn towards the new bearing
  moveRobot(newBearing, true);
}

void rotateInPlace() {
  // Rotate in place by turning left or right
  leftServo.writeMicroseconds(MIN_SPEED); // Left motor backward
  rightServo.writeMicroseconds(MAX_SPEED); // Right motor forward
}

void stopRobot() {
  leftServo.writeMicroseconds(STOP); // Stop both servos
  rightServo.writeMicroseconds(STOP);
}
