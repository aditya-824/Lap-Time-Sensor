const int TRIG_PIN = 8;
const int ECHO_PIN = 9;
float uTiming = 0.0;
float uDistance = 0.0;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TRIG_PIN, LOW);
  delay(2);

  digitalWrite(TRIG_PIN, HIGH);
  delay(10);  
  digitalWrite(TRIG_PIN, LOW);

  uTiming = pulseIn(ECHO_PIN, HIGH);
  uDistance = (uTiming * 0.034) / 2;

  Serial.print("Distance: ");
  Serial.print(uDistance);
  Serial.println("cm");

  delay(100);
}
