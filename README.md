# 🗑️ Smart Waste Management System — IoT Smart Bin

> Real-time bin fill monitoring with ESP32, Firebase, and a live web dashboard.

![Dashboard Preview](assests/dashboard.png)
![Dashboard Preview](assests/dashboard2.png)

---

## 📌 Overview

The **Smart Waste Management System** is an IoT solution that uses an **ESP32 microcontroller** and an **HC-SR04 ultrasonic sensor** to measure waste bin fill levels. Data is streamed live to **Firebase Realtime Database** and visualized through a responsive **web dashboard** built with HTML, Tailwind CSS, and Chart.js.

### Key Benefits
- 📡 Real-time bin fill level monitoring
- 🔔 Automatic alerts when bins approach full capacity
- 📊 Historical trend visualization for smarter waste management decisions
- 🌐 Deployable on GitHub Pages — no server required

---

## 🏗️ System Architecture

```
Ultrasonic Sensor (HC-SR04)
         │
         ▼
   ESP32 Microcontroller
         │  (Wi-Fi)
         ▼
Firebase Realtime Database
         │
         ▼
 Web Dashboard (HTML + Tailwind + Chart.js)
```

---

## 🛠️ Technologies Used

### Hardware
| Component | Purpose |
|-----------|---------|
| ESP32 | Wi-Fi-enabled microcontroller |
| HC-SR04 | Ultrasonic distance sensor |

### Software & Services
| Technology | Role |
|------------|------|
| HTML / Tailwind CSS | Dashboard UI |
| Chart.js | Data visualization |
| JavaScript | Dashboard logic |
| Firebase Realtime Database | Cloud data storage & sync |
| Arduino IDE | ESP32 firmware development |
| Wokwi IoT Simulator | Hardware simulation & testing |

---

## 📂 Repository Structure

```
Smart-Waste-Management-IoT-Smart-Bin-Real-Hardware-Dashboard-/
│
├── SWM_system/              # ESP32 firmware (Arduino IDE)
├── SWM-simulator/           # Simulation files
├── dustbin wokwi/           # Wokwi simulation project
├── assests/                 # Dashboard screenshots
├── index.html               # Live web dashboard
├── beep.mp3                 # Alert alarm sound
└── README.md
```

---

## 📊 Dashboard Features

### 🔴 Real-Time Monitoring
- Displays the current bin fill percentage
- Automatically syncs with Firebase — no page refresh needed

### 📈 Data Visualization
- Line chart showing recent bin fill history
- Weekly waste statistics at a glance

### ⚠️ Alert System

| Fill Level | Status |
|------------|--------|
| > 80% | ⚠️ Warning — Bin almost full |
| > 90% | 🚨 Critical alert + audible alarm |

### 🎛️ Interactive Controls
- Enable / disable alarm toggle
- Color-coded visual indicators for bin status

---

## 🔥 Firebase Database Structure

The ESP32 writes data to the following path:

```
/dustbin
    └── filled_percentage: 0–100
```

**Example JSON:**
```json
{
  "dustbin": {
    "filled_percentage": 72
  }
}
```

---

## 💻 ESP32 Firmware

Example Arduino sketch for sending bin fill data to Firebase:

```cpp
#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "your-project-id-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret"

FirebaseData fbdo;

void setup() {
  Serial.begin(115200);
  WiFi.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Replace with actual HC-SR04 distance-to-percentage calculation
  float filled = random(0, 100);

  if (Firebase.setFloat(fbdo, "/dustbin/filled_percentage", filled)) {
    Serial.println("Data sent: " + String(filled) + "%");
  } else {
    Serial.println("Firebase error: " + fbdo.errorReason());
  }

  delay(5000);
}
```

---

## 🔌 Hardware Wiring

| HC-SR04 Pin | ESP32 Pin | Notes |
|-------------|-----------|-------|
| VCC | 5V | — |
| GND | GND | — |
| TRIG | GPIO 5 | — |
| ECHO | GPIO 18 | ⚠️ Use voltage divider if sensor outputs 5V |

> **⚠️ Important:** The HC-SR04 ECHO pin outputs 5V. Since ESP32 GPIO is 3.3V tolerant, use a voltage divider (e.g., 1kΩ + 2kΩ resistors) on the ECHO line to protect the microcontroller.

---

## 🧪 Wokwi Simulation

The project can be fully tested in the **[Wokwi IoT Simulator](https://wokwi.com)** before deploying on real hardware.

**Simulation includes:**
- Virtual ESP32 board
- Ultrasonic sensor with adjustable distance slider
- Real-time fill level feedback

This lets you validate firmware logic and Firebase integration without any physical components.

---

## 🚀 Deployment (GitHub Pages)

The dashboard is a static web app and can be hosted for free on **GitHub Pages**.

**Steps:**
1. Open your repository on GitHub
2. Go to **Settings → Pages**
3. Under *Branch*, select `main` and click **Save**
4. Your dashboard will be live at:

```
https://<your-username>.github.io/Smart-Waste-Management-IoT-Smart-Bin-Real-Hardware-Dashboard-/
```

---

## 🔒 Security Recommendations

This project exposes Firebase configuration in the client-side dashboard. For a **production deployment**, apply the following hardening steps:

- [ ] Restrict Firebase Realtime Database rules to read-only for unauthenticated users
- [ ] Enable Firebase Authentication and require login for write access
- [ ] Move sensitive keys to environment variables or a backend proxy
- [ ] Avoid committing API keys or database secrets to public repositories

---

## 📄 License

This project is open-source. Feel free to use, modify, and distribute it with attribution.

---

