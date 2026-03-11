
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/RTDBHelper.h>

// WiFi Credentials
#define WIFI_SSID "SLTC_LEARN"
#define WIFI_PASSWORD "Success@SLTC"

// Firebase Credentials
#define DATABASE_URL "https://swm-system-6b210-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define DATABASE_SECRET "GqfE7Dktd9K2Tts6jzookltJCbKh95TsPjJTxgSz"

// Pins
#define TRIG_PIN 26
#define ECHO_PIN 32

// Bin constants
#define DUSTBIN_HEIGHT_CM 16.94

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {

  Serial.begin(115200);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected!");

  // Firebase configuration
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float getDistanceCM() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  float distance = (duration * 0.0343) / 2.0;

  return distance;
}

void loop() {

  float distance = getDistanceCM();

  float filledPercent =
      ((DUSTBIN_HEIGHT_CM - distance) / DUSTBIN_HEIGHT_CM) * 100.0;

  if (filledPercent < 0) filledPercent = 0;
  if (filledPercent > 100) filledPercent = 100;

  String status = "EMPTY";

  if (filledPercent > 80)
    status = "FULL";
  else if (filledPercent > 50)
    status = "HALF";

  unsigned long currentTime = millis() / 1000;

  // Send data to Firebase
  Firebase.RTDB.setFloat(&firebaseData, "/dustbin/distance_cm", distance);
  Firebase.RTDB.setFloat(&firebaseData, "/dustbin/filled_percentage", filledPercent);
  Firebase.RTDB.setString(&firebaseData, "/dustbin/status", status);
  Firebase.RTDB.setFloat(&firebaseData, "/dustbin/dustbin_height_cm", DUSTBIN_HEIGHT_CM);
  Firebase.RTDB.setInt(&firebaseData, "/dustbin/last_updated", currentTime);

  Serial.printf("Distance: %.2f cm | Fill: %.2f%% | Status: %s\n",
                distance, filledPercent, status.c_str());

  delay(5000);
}