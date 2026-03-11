# Smart Waste Management (IoT) — Smart Bin (Real Hardware + Dashboard)

This repository contains a Smart Waste Management system: an ESP32 measuring bin fill level (ultrasonic) and streaming telemetry to Firebase Realtime Database. The dashboard is an HTML + Tailwind + Chart.js frontend that reads Firebase and shows live telemetry, weekly charts and alerts.

Table of contents
- Overview
- Dashboard (what it expects)
- How firmware should push telemetry (minimal example)
- Hardware & wiring
- Wokwi simulation notes
- Deployment (GitHub Pages)
- Security notes & next steps
- Contributors

Overview
- ESP32 measures distance, converts to fill percentage (0–100) and writes it to Realtime Database under `/dustbin/filled_percentage`.
- Dashboard listens to `/dustbin` and reads `filled_percentage`. It shows trending charts, weekly totals and sounds an alarm when thresholds are exceeded.

Dashboard (details & behavior)
- Files:
  - index.html — main dashboard (uses Firebase compat v9, Tailwind, Lucide, Chart.js)
  - beep.mp3 — alarm sound referenced by index.html (must be present on host)
- Firebase config in index.html (as used by the working dashboard):
  - apiKey, authDomain, databaseURL: https://swm-system-6b210-default-rtdb.asia-southeast1.firebasedatabase.app
  - projectId: swm-system-6b210
- Database path used:
  - db.ref("dustbin") → dashboard expects `filled_percentage` field (number between 0 and 100)
- Visual logic:
  - Line chart shows last ~20 timestamps of `filled_percentage`.
  - Weekly bar chart (Organic) uses calculated kg = (filled_percentage / 100) * maxKg where maxKg is set to 16 in the dashboard.
  - Alerts:
    - filled_percentage > 80 → warning "Bin Full"
    - filled_percentage > 90 → critical alarm "Bin Critically Full"
  - Alarm requires user interaction on some browsers (catching play() promise) — the dashboard uses .play().catch(...) and a toggle button.

How firmware should push telemetry (ESP32 example)
- Goal: write a numeric 0–100 value to `/dustbin/filled_percentage`.

Minimal Arduino (ESP32) example using FirebaseESP32 (Mobizt library)
- Install:
  - Add ESP32 board support to Arduino IDE
  - Install the FirebaseESP32 library (Mobizt)
- Example (replace placeholders with actual values like WIFI_SSID, FIREBASE_HOST and FIREBASE_AUTH):

```cpp name=firmware_example.ino
#include <WiFi.h>
#include <FirebaseESP32.h>

// replace with your WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Firebase settings
#define FIREBASE_HOST "swm-system-6b210-default-rtdb.asia-southeast1.firebasedatabase.app" // host (no https://)
#define FIREBASE_AUTH "YOUR_DATABASE_SECRET_OR_AUTH_TOKEN" // if using database secrets or a token

// HC-SR04 pins
#define TRIG_PIN 5
#define ECHO_PIN 18
const int BIN_HEIGHT_CM = 40; // adjust to your bin height

FirebaseData fbdo;

long readUltrasonicCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  if(duration == 0) return -1;
  long distance = duration / 29 / 2;
  return distance;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  long distance = readUltrasonicCM(TRIG_PIN, ECHO_PIN);
  if(distance > 0) {
    // convert distance->filled_percentage (0..100)
    // clamp distance between 0..BIN_HEIGHT_CM
    long clamped = max(0L, min((long)BIN_HEIGHT_CM, distance));
    float filled = (1.0 - (float)clamped / BIN_HEIGHT_CM) * 100.0;
    if(filled < 0) filled = 0;
    if(filled > 100) filled = 100;

    Serial.printf("Distance: %ld cm → filled: %.1f %%\n", distance, filled);
    // write to /dustbin/filled_percentage
    if (Firebase.setFloat(fbdo, "/dustbin/filled_percentage", filled)) {
      Serial.println("Uploaded filled_percentage to Firebase");
    } else {
      Serial.printf("Firebase write failed: %s\n", fbdo.errorReason().c_str());
    }
  } else {
    Serial.println("Ultrasonic read failed (timeout)");
  }
  delay(5000); // adjust sample interval
}
```

Notes about authentication & security
- Dashboard has firebaseConfig with an apiKey and databaseURL. If this repo is public you should secure your Realtime Database rules to prevent unauthorized writes.
- Recommended flow for production:
  - Secure DB rules and use authenticated users or a backend to mediate writes.
  - If leaving the DB public for testing, ensure it’s temporary and limit exposure.

Hardware & wiring (HC‑SR04 → ESP32)
- TRIG -> GPIO 5
- ECHO -> GPIO 18 (use voltage divider if Echo is 5V)
- VCC -> 5V (or 3.3V if your module supports it)
- GND -> GND
- BIN_HEIGHT_CM constant in the sketch should reflect the distance from sensor to bottom of bin.

Wokwi simulation
- Wokwi provides an HC‑SR04 slider and ESP32 simulation. Match TRIG/ECHO pins and test logic before deploying.

Deployment (static host)
- Deploy index.html and beep.mp3 to GitHub Pages or any static host.
- Ensure firebaseConfig is present or replaced by a secure access method.

Contributors (as shown in dashboard footer)
- Chamathvi Kawya — ID: 22UG2-0193
- Chamika Raigama — ID: 22UG2-0567

Next steps I can help with
- Create a production-ready firmware that posts multiple bins (organic/plastic/metal) with distinct paths (e.g., `/bins/organic/filled_percentage`) if you plan to monitor multiple nodes.
- Harden Firebase rules or show how to use REST + a secure token.
- Remove firebaseConfig from a public repo and show how to load it from a server/config file.
- Prepare repository structure and commit these files / open a PR if you give the repo owner/name.
