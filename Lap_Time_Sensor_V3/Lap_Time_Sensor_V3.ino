/* LIBRARIES */
#include <stdbool.h>

/* TIME DEFINES */
#define carDelay 5 // Delay for car to pass by in seconds
#define startDelay 5 // Delay to prevent start of measurement at end of measurement, in seconds
#define MAX_LAPS 2 // Maximum number of laps that can be measured
#define DISTANCE_THRES 10.0 // Distance between car and sensor at start/finish line

/* PINS */
// const int motionSensor = 14;
const int TRIG_PIN = 8;
const int ECHO_PIN = 9;

// Timer: Auxiliary variables
volatile unsigned long startTime = 0; // Stores the start time
volatile unsigned long endTime = 0; // Stores the end time
volatile boolean firstSignalDetected = false; // Indicates whether high signal is first detected or not
volatile boolean timingComplete = false; // Indicates whether one timing cycle has completed or not
volatile boolean timingStarted = false; // Indicates when timing has started
int reset = 0; // Switch to reset timing code
volatile unsigned long lastDetectionTime = 0; // Stores the latest measured time for timed high-signal deadbands
float uTiming = 0.0; // Ultrasonic sensor timing
float uDistance = 0.0; // Ultrasonic sensor distance
unsigned long fastestTime = 0; // Fastest lap time
unsigned long slowestTime = 0; // Slowest lap time
unsigned long lapTimes[MAX_LAPS] = {0}; // Array to store lap times
volatile int lapCount = 0; // Number of laps recorded

/* WIFI PARAMETERS */
char ssid[] = "";
char password[] = "";

void detectsMovement() { // Interrupt function
  unsigned long now = millis();
  if (lapCount >= MAX_LAPS) return; // Stop detecting after set laps

  if (!firstSignalDetected) { // Starts timing first lap
    startTime = now;
    firstSignalDetected = true;
    lastDetectionTime = now;
    timingStarted = true;
  } else if (now - lastDetectionTime >= carDelay*1000) { // Ends timing cycle if at least 5 seconds have passed since start of cycle
    endTime = now;
    // firstSignalDetected = false;
    timingComplete = true;
    timingStarted = true;
    lastDetectionTime = now;
  }
}

// BLYNK_WRITE(V1) // Blynk callback function to send value of Reset button
// {
//   reset = param.asInt(); // Get value as integer
// }

void setup() {
  Serial.begin(9600);

  // pinMode(motionSensor, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  digitalWrite(TRIG_PIN, LOW);

  // Blynk.begin (BLYNK_AUTH_TOKEN, ssid, password); // NOTE: Code will not progress until Wi-Fi connection is established
}

void loop() {
  // Blynk.run();

  digitalWrite(TRIG_PIN, LOW);
  delay(2);
  digitalWrite(TRIG_PIN, HIGH);
  delay(10);
  digitalWrite(TRIG_PIN, LOW); // Generating ultrasonic wave
  uTiming = pulseIn(ECHO_PIN, HIGH);
  uDistance = (uTiming * 0.034) / 2; // Calculating distance from sensor

  if (reset) { // Resets timer if virtual pin is high
    lapCount = 0;
    firstSignalDetected = false;
    reset = 0;
    delay(5000); // Wait for virtual pin to reset
    Serial.println("Timer reset.");
    Serial.println("---------------------------------------------------------------------------");
  }

  if (uDistance <= DISTANCE_THRES) {
    detectsMovement();
  }

  if (timingComplete && lapCount < MAX_LAPS) {
    unsigned long duration = endTime - startTime;
    startTime = millis(); // Starts timing for next lap
    lapTimes[lapCount] = duration;
    Serial.print("Lap ");
    Serial.print(lapCount + 1);
    Serial.println(" timing stopped.");
    Serial.print("Time: ");
    Serial.print(duration);
    Serial.println(" ms");
    // Blynk.virtualWrite(V0, duration); // Send current lap time to Blynk
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
        Serial.print(lapTimes[i]);
        Serial.println(" ms");
        if(lapTimes[i] < fastestTime) {
          fastestTime = lapTimes[i];
        }
        if(lapTimes[i] > slowestTime) {
          slowestTime = lapTimes[i];
        }
      }
      Serial.print("Fastest time: ");
      Serial.println(fastestTime);
      Serial.print("Slowest time: ");
      Serial.println(slowestTime);
      Serial.println("Reset module to restart timer.");
      // Optionally, send all lap times to Blynk here
    }
  }

  if (timingStarted) { // Kept below to print completed lap timing before indicating new lap starting
    Serial.print("Lap ");
    Serial.print(lapCount + 1);
    Serial.println(" timing started...");
    timingStarted = false;
  }
}