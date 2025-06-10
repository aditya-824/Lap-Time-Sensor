# Lap-Time-Sensor
NodeMCU ESP8266 based lap time sensor for 2025 prototype

### Sensor Pins:
- 5V supply
- TRIG_PIN: Ultrasonic sensor Trigger pin
- ECHO_PIN: Ultrasonic sensor Echo pin
- Ground

### Parameters:
- CAR_DELAY: Delay for the rest of the car to pass by after it is first detected by the sensor, in seconds
- MAX_LAPS: Number of laps to time
- DISTANCE_THRES: Distance between car and sensor at start/finish line that will trigger the sensor, in cm
- Reset timer by resetting Arduino

## V1
- Basic pulse-activated timer that sends timed value to Blynk web dashboard

## V2
- Measurement of multiple lap times, yet to made consecutive
- Timer made resettable via Blynk virtual pin

## V3
- Integrated ultrasonic sensor
- Distance based threshold selected for detecting car
- Lap timing made consecutive
- Dropped Blynk functionality
- Calculating highest & lowest lap times

## V4
- Formatted time
- Reset function implemented by resetting board
- Final version of code

## Ultrasonic_Sensor_Test
- Arduino sketch to get direct distance readings from ultrasonic sensor