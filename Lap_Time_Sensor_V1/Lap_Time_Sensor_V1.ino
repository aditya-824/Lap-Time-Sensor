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

// Set GPIOs for LED and PIR Motion Sensor
const int motionSensor = 14;

// Timer: Auxiliary variables
volatile unsigned long startTime = 0; // Stores the start time
volatile unsigned long endTime = 0; // Stores the end time
volatile boolean firstSignalDetected = false; // Indicates whether high signal is first detected or not
volatile boolean timingComplete = false; // Indicates whether one timing cycle has completed or not
volatile boolean timingStarted = false; // Indicates when timing has started
volatile unsigned long lastDetectionTime = 0; // Stores the latest measured time for timed high-signal deadbands

/* WIFI PARAMETERS */
char ssid[] = "Aditya's Phone";
char password[] = "geO071!?";


void ICACHE_RAM_ATTR detectsMovement() { // Interrupt function
  unsigned long now = millis();
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

void setup() {
  Serial.begin(9600);

  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  Blynk.begin (BLYNK_AUTH_TOKEN, ssid, password); // NOTE: Code will not progress until Wi-Fi connection is established
}

void loop() {
  Blynk.run();

  if (timingStarted) {
    Serial.println("Timing started...");
    timingStarted = false;
  }

  if (timingComplete) {
    unsigned long duration = endTime - startTime;
    Serial.println("Timing stopped.");
    Serial.print("Time: ");
    Serial.print(duration);
    Serial.println(" ms");
    timingComplete = false;
    Blynk.virtualWrite(V0, duration); // Sends 'duration' value to Blynk virtual pin
  }
}