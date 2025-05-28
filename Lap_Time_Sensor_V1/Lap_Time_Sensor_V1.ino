#define BLYNK_TEMPLATE_ID "TMPL3SyT65zC5"
#define BLYNK_TEMPLATE_NAME "Lap Time Sensor"
#define BLYNK_AUTH_TOKEN "9xpbf-HVRUJnluJW_mOhDdkJHhoJ5mn7"

#include <stdbool.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#define carDelay 5 // Delay for car to pass by in seconds
#define startDelay 2 // Delay to prevent start of measurement at end of measurement, in seconds

// Set GPIOs for LED and PIR Motion Sensor
const int motionSensor = 14;

// Timer: Auxiliary variables
volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile boolean firstSignalDetected = false;
volatile boolean timingComplete = false;
volatile unsigned long lastDetectionTime = 0;
volatile boolean timingStarted = false;

char ssid[] = "Aditya's Phone";
char password[] = "geO071!?";

void ICACHE_RAM_ATTR detectsMovement() {
  unsigned long now = millis();
  if (!firstSignalDetected && ((now - lastDetectionTime) >= startDelay*1000)) {
    startTime = now;
    firstSignalDetected = true;
    lastDetectionTime = now;
    timingStarted = true;
  } else if (now - lastDetectionTime >= carDelay*1000) {
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

  Blynk.begin (BLYNK_AUTH_TOKEN, ssid, password);
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
    Blynk.virtualWrite(V0, duration);
  }
}