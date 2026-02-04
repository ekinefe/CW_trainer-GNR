# CW_trainer-GNR

[cite_start]**CW_trainer-GNR** is a high-performance, cross-platform Morse Code (CW) training station developed with **C++ and the Qt 6 Framework**[cite: 1]. [cite_start]Designed for both software-only practice and hardware-integrated training, it provides a data-driven path to mastering Morse Code from beginner to advanced levels[cite: 1].

---

## 📖 Table of Contents
* [🎯 Goal of the Project](#-goal-of-the-project)
* [🚀 Key Features](#-key-features)
* [🕹 User Manual](#-user-manual)
* [📈 Checking Analytics](#-checking-analytics)
* [🔌 Hardware Integrity & Integration](#-hardware-integrity--integration)
* [📥 How to Download](#-how-to-download)
* [💻 Detailed Platform Setup](#-detailed-platform-setup)

---

## 🎯 Goal of the Project
[cite_start]The primary objective of CW_trainer-GNR is to provide a comprehensive, algorithmic environment for learning Morse Code[cite: 1]. [cite_start]By utilizing the **Koch Method** and real-time **Echo Practice**, the application focuses on character recognition at high speeds, preventing the "plateau" effect common in traditional learning[cite: 1]. [cite_start]It aims to offer a "professional station" feel on a standard desktop computer[cite: 1].

---

## 🚀 Key Features

### 1. Adaptive Training Modes
* [cite_start]**Random Characters:** Generates alphanumeric strings to build raw decoding speed[cite: 1].
* [cite_start]**Koch Method:** Progressively introduces characters (starting with K and M) once a 90% accuracy threshold is met[cite: 1].
* [cite_start]**Echo Practice:** An interactive "Call & Response" mode where you must mimic the app's output[cite: 1].
* [cite_start]**TX (Transmit) Mode:** Interfaces with physical paddles to verify your sending rhythm and accuracy[cite: 1].

### 2. Signal Precision
* [cite_start]**Paris Standard:** Timing logic follows the international "PARIS" standard for dots, dashes, and spacing[cite: 1].
* [cite_start]**Customization:** User-adjustable speed (5–50 WPM) and pitch (300Hz–1200Hz)[cite: 1].



---

## 🕹 User Manual

### How to Start
1. [cite_start]**Selection:** Choose your desired mode from the side menu[cite: 1].
2. [cite_start]**Configuration:** Use the sliders to set your target **WPM (Words Per Minute)** and **Tone Frequency**[cite: 1].
3. **Execution:** Press the **Start** button.
4. **Input:** When you hear a tone, type the corresponding key on your keyboard. 
    * [cite_start]**Green Highlight:** Correct input[cite: 1].
    * [cite_start]**Red Highlight:** Incorrect input (the correct character will be shown)[cite: 1].
5. [cite_start]**Stop:** Press **Stop** to end the session and save your data[cite: 1].

---

## 📈 Checking Analytics
The application treats your progress like an athlete's performance data.

1. [cite_start]**Trend Charts:** Navigate to the **Statistics** window to see your accuracy trend over time[cite: 1].
2. [cite_start]**Character Analysis:** A bar chart identifies your "Problem Characters"—those with the highest error rates[cite: 1].
3. [cite_start]**Persistent Logs:** All data is saved in a `statistics.csv` file in the application folder[cite: 1]. This allows you to track progress over months or even years.



---

## 🔌 Hardware Integrity & Integration
For a truly authentic experience, CW_trainer-GNR supports external hardware keyers.

* [cite_start]**Connection:** Plug your Arduino/ESP32 keyer into your USB port[cite: 1].
* [cite_start]**Integrity Check:** The app uses a **Serial Manager** to monitor the connection status in real-time[cite: 1]. If the device is disconnected, the app will alert you.
* [cite_start]**Settings:** Ensure your hardware is set to **115200 Baud**[cite: 1].
* **TX Mapping:** In TX mode, the app decodes the incoming Serial characters and compares them against the target string to grade your manual sending.

---

## 📥 How to Download
You can find pre-built binaries in the **Download/** directory of this repository:

* [cite_start]**Windows:** Download the `.exe` for installation or `.zip` for portable use[cite: 1].
* [cite_start]**Linux:** Download the `.AppImage` for a "no-install" experience on any distribution[cite: 1].

---

## 💻 Detailed Platform Setup

### 🪟 Windows
1. [cite_start]**Installer Method:** Run `CW_Trainer-GNR_vX.X.X_Setup.exe`[cite: 1]. It will create desktop shortcuts and an uninstaller.
2. [cite_start]**Portable Method:** Extract `CW_Trainer-GNR_vX.X.X_Portable.zip`[cite: 1]. Run `CW_Trainer-GNR.exe` inside. 

### 🐧 Linux
1. [cite_start]Download the `.AppImage`[cite: 1].
2. Right-click the file -> **Properties** -> **Permissions** -> **Allow executing file as program**.
3. [cite_start]Alternatively, run in terminal: `chmod +x CW_Trainer-GNR*.AppImage && ./CW_Trainer-GNR*.AppImage`[cite: 1].

### 🍎 macOS
*Currently available via Source Build:*
1. Install **Qt 6** and **Qt Creator**.
2. [cite_start]Open `CW_Trainer-GNR.pro`[cite: 1].
3. Select the **Desktop Qt 6 (clang)** kit and press **Run**.

---

**Developed with ❤️ for the CW Community.**
