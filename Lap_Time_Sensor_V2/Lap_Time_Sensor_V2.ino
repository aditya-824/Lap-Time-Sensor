/* BLYNK MANDATED */
#define BLYNK_TEMPLATE_ID "TMPL3SyT65zC5"
#define BLYNK_TEMPLATE_NAME "Lap Time Sensor"
#define BLYNK_AUTH_TOKEN "9xpbf-HVRUJnluJW_mOhDdkJHhoJ5mn7"

/* LIBRARIES */
#include <stdbool.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

/* TIME DEFINES */
#define carDelay 5 // Delay for car to pass by in seconds
#define startDelay 2 // Delay to prevent start of measurement at end of measurement, in seconds
#define MAX_LAPS 2 // Maximum number of laps that can be measured

// Set GPIOs for LED and PIR Motion Sensor
const int motionSensor = 14;

// Timer: Auxiliary variables
volatile unsigned long startTime = 0; // Stores the start time
volatile unsigned long endTime = 0; // Stores the end time
volatile boolean firstSignalDetected = false; // Indicates whether high signal is first detected or not
volatile boolean timingComplete = false; // Indicates whether one timing cycle has completed or not
volatile boolean timingStarted = false; // Indicates when timing has started
volatile boolean reset = false; // Switch to reset timing code
volatile unsigned long lastDetectionTime = 0; // Stores the latest measured time for timed high-signal deadbands

unsigned long lapTimes[MAX_LAPS] = {0}; // Array to store lap times
volatile int lapCount = 0; // Number of laps recorded

/* WIFI PARAMETERS */
char ssid[] = "";
char password[] = "";

void ICACHE_RAM_ATTR detectsMovement() { // Interrupt function
  unsigned long now = millis();
  if (lapCount >= MAX_LAPS) return; // Stop detecting after set laps

  if (!firstSignalDetected && ((now - lastDetectionTime) >= startDelay*1000)) { // Starts timing if high signal is first in cycle and 2 seconds have passed since end of last cycle
    startTime = now;
    firstSignalDetected = true;
    lastDetectionTime = now;
    timingStarted = true;
  } else if (now - lastDetectionTime >= carDelay*1000) { // Ends timing cycle if at least 5 seconds have passed since start of cycle
    endTime = now;
    firstSignalDetected = false;
    timingComplete = true;
    lastDetectionTime = now;
  }
}

BLYNK_WRITE(V1) // Blynk callback function to send value of Reset button
{
  reset = param.asInt(); // Get value as integer
}

void setup() {
  Serial.begin(9600);

  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  Blynk.begin (BLYNK_AUTH_TOKEN, ssid, password); // NOTE: Code will not progress until Wi-Fi connection is established
}

void loop() {
  Blynk.run();

  if (reset) { // Resets timer if virtual pin is high
    lapCount = 0;
    delay(5000); // Wait for virtual pin to reset
  }

  if (timingStarted) {
    Serial.print("Lap ");
    Serial.print(lapCount + 1);
    Serial.println(" timing started...");
    timingStarted = false;
  }

  if (timingComplete && lapCount < MAX_LAPS) {
    unsigned long duration = endTime - startTime;
    lapTimes[lapCount] = duration;
    Serial.print("Lap ");
    Serial.print(lapCount + 1);
    Serial.println(" timing stopped.");
    Serial.print("Time: ");
    Serial.print(duration);
    Serial.println(" ms");
    Blynk.virtualWrite(V0, duration); // Send current lap time to Blynk
    lapCount++;
    timingComplete = false;

    if (lapCount == MAX_LAPS) {
      Serial.print(MAX_LAPS);
      Serial.println(" laps completed. Lap times:");
      for (int i = 0; i < MAX_LAPS; i++) {
        Serial.print("Lap ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(lapTimes[i]);
        Serial.println(" ms");
      }
      // Optionally, send all lap times to Blynk here
    }
  }
}