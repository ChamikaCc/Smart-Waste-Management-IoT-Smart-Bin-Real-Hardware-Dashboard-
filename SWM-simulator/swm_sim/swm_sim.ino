#include <Arduino.h>

// Pins for 3 ultrasonic sensors
#define TRIG_ORG 26
#define ECHO_ORG 32

#define TRIG_PL 25
#define ECHO_PL 33

#define TRIG_MET 27
#define ECHO_MET 34

#define DUSTBIN_HEIGHT_CM 16.94

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_ORG, OUTPUT); pinMode(ECHO_ORG, INPUT);
  pinMode(TRIG_PL, OUTPUT);  pinMode(ECHO_PL, INPUT);
  pinMode(TRIG_MET, OUTPUT); pinMode(ECHO_MET, INPUT);
}

// ---------- Read distance ----------
float getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  float distance = (duration * 0.0343) / 2.0;
  return distance;
}

// ---------- Calculate fill percentage ----------
float calculateFill(float distance) {
  float fill = ((DUSTBIN_HEIGHT_CM - distance) / DUSTBIN_HEIGHT_CM) * 100.0;
  if(fill < 0) fill = 0;
  if(fill > 100) fill = 100;
  return fill;
}

// ---------- Loop ----------
void loop() {
  float fillOrg = calculateFill(getDistanceCM(TRIG_ORG, ECHO_ORG));
  float fillPl  = calculateFill(getDistanceCM(TRIG_PL, ECHO_PL));
  float fillMet = calculateFill(getDistanceCM(TRIG_MET, ECHO_MET));

  // Print in a structured format
  // Format: BIN1:45,BIN2:32,BIN3:78
  Serial.printf("BIN1:%.1f,BIN2:%.1f,BIN3:%.1f\n", fillOrg, fillPl, fillMet);

  delay(2000); // Update every 2 seconds
}