/* LIBRARIES */
#include <stdbool.h>

/* TIME DEFINES */
#define CAR_DELAY 5 // Delay for car to pass by, in seconds
#define MAX_LAPS 4 // Number of laps to measure
#define DISTANCE_THRES 100.0 // Distance between car and sensor at start/finish line, in cm

/* PINS */
const int TRIG_PIN = 8; // Ultrasonic sensor Trigger pin
const int ECHO_PIN = 9; // Ultrasonic sensor Echo pin

/* VARIABLES */
volatile unsigned long startTime = 0; // Stores the start time
volatile unsigned long endTime = 0; // Stores the end time
volatile boolean firstSignalDetected = false; // Indicates whether high signal is first detected or not
volatile boolean timingComplete = false; // Indicates whether one timing cycle has completed or not
volatile boolean timingStarted = false; // Indicates when timing has started
volatile unsigned long lastDetectionTime = 0; // Stores the latest measured time for timed high-signal deadbands
float uTiming = 0.0; // Ultrasonic sensor timing
float uDistance = 0.0; // Ultrasonic sensor distance
unsigned long fastestTime = 0; // Fastest lap time
unsigned long slowestTime = 0; // Slowest lap time
unsigned long lapTimes[MAX_LAPS] = {0}; // Array to store lap times
volatile int lapCount = 0; // Number of laps recorded
volatile int mins = 0;
volatile int secs = 0;
volatile int millisecs = 0;

/* FUNCTIONS */
void detectsMovement() { // Timing function
  unsigned long now = millis();
  if (lapCount >= MAX_LAPS) return; // Stop detecting after set laps

  if (!firstSignalDetected) { // Starts timing first lap
    startTime = now;
    firstSignalDetected = true;
    lastDetectionTime = now;
    timingStarted = true;
  } else if (now - lastDetectionTime >= CAR_DELAY*1000) { // Ends timing cycle if at least 5 seconds have passed since start of cycle
    endTime = now;
    timingComplete = true;
    timingStarted = true;
    lastDetectionTime = now;
  }
}

void formatTime(int time) { // Formats time in ms to mins, secs and ms
  mins = time / 6000;
  secs = time / 1000;
  millisecs = time - (mins * 6000) - (secs * 1000);
  Serial.print(mins);
  Serial.print(":");
  Serial.print(secs);
  Serial.print(":");
  Serial.println(millisecs);
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);
  delay(500);
  Serial.println("Sensor configured.");
  Serial.println("---------------------------------------------------------------------------");
}

void loop() {
  /* Generating ultrasonic wave */
  digitalWrite(TRIG_PIN, LOW);
  delay(2);
  digitalWrite(TRIG_PIN, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN, LOW);
  uTiming = pulseIn(ECHO_PIN, HIGH);
  uDistance = (uTiming * 0.034) / 2; // Calculating distance from sensor

  if (uDistance <= DISTANCE_THRES) {
    detectsMovement(); // Calling function when car passes by
  }

  if (timingComplete && lapCount < MAX_LAPS) {
    unsigned long duration = endTime - startTime;
    startTime = millis(); // Starts timing for next lap
    lapTimes[lapCount] = duration;
    Serial.print("Lap ");
    Serial.print(lapCount + 1);
    Serial.println(" timing stopped.");
    Serial.print("Time: ");
    formatTime(duration); // Formatting and printing time
    lapCount++;
    timingComplete = false;

    if (lapCount == MAX_LAPS) {
      timingStarted = false; // To prevent timingStarted conditional running after
      fastestTime = lapTimes[0];
      slowestTime = lapTimes[0];
      Serial.println("---------------------------------------------------------------------------");
      Serial.print(MAX_LAPS);
      Serial.println(" laps completed. Lap times:");
      for (int i = 0; i < MAX_LAPS; i++) {
        Serial.print("Lap ");
        Serial.print(i + 1);
        Serial.print(": ");
        formatTime(lapTimes[i]);
        if(lapTimes[i] < fastestTime) { // Calculating fastest lap
          fastestTime = lapTimes[i];
        }
        if(lapTimes[i] > slowestTime) { // Calculating slowest lap
          slowestTime = lapTimes[i];
        }
      }
      Serial.print("Fastest time: ");
      formatTime(fastestTime);
      Serial.print("Slowest time: ");
      formatTime(slowestTime);
      Serial.println("Reset Arduino to restart timer.");
      Serial.println("---------------------------------------------------------------------------");
    }
  }

  if (timingStarted) { // Kept below to print completed lap timing before indicating new lap starting
    Serial.print("Lap ");
    Serial.print(lapCount + 1);
    Serial.println(" timing started...");
    timingStarted = false;
  }
}