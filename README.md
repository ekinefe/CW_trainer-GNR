# CW_trainer-GNR

**CW_trainer-GNR** is a high-performance, cross-platform Morse Code (CW) training station developed with **C++ and the Qt 6 Framework**. Designed for both software-only practice and hardware-integrated training, it provides a data-driven path to mastering Morse Code from beginner to advanced levels.

---

## 📖 Table of Contents
* [🎯 Goal of the Project](#-goal-of-the-project)
* [🚀 Key Features](#-key-features)
* [🕹 User Manual](#-user-manual)
* [📈 Analytics](#-analytics)
* [🔌 Hardware Integrity](#-hardware-integrity)
* [📥 How to Download](#-how-to-download)
* [💻 Platform Setup](#-platform-setup)

---

## 🎯 Goal of the Project
The primary objective of CW_trainer-GNR is to provide a comprehensive, algorithmic environment for learning Morse Code. By utilizing the **Koch Method** and real-time **Echo Practice**, the application focuses on character recognition at high speeds, preventing the "plateau" effect common in traditional learning. It aims to offer a "professional station" feel on a standard desktop computer.

---

## 🚀 Key Features

### 1. Adaptive Training Modes
* **Random Characters:** Generates alphanumeric strings to build raw decoding speed.
* **Koch Method:** Progressively introduces characters (starting with K and M) once a 90% accuracy threshold is met.
* **Echo Practice:** An interactive "Call & Response" mode where you must mimic the app's output.
* **TX (Transmit) Mode:** Interfaces with physical paddles to verify your sending rhythm and accuracy.

### 2. Signal Precision
* **Paris Standard:** Timing logic follows the international "PARIS" standard for dots, dashes, and spacing.
* **Customization:** User-adjustable speed (5–50 WPM) and pitch (300Hz–1200Hz).



---

## 🕹 User Manual

### How to Start
1. **Selection:** Choose your desired mode from the side menu.
2. **Configuration:** Use the sliders to set your target **WPM (Words Per Minute)** and **Tone Frequency**.
3. **Execution:** Press the **Start** button.
4. **Input:** When you hear a tone, type the corresponding key on your keyboard. 
    * **Green Highlight:** Correct input.
    * **Red Highlight:** Incorrect input (the correct character will be shown).
5. **Stop:** Press **Stop** to end the session and save your data.

---

## 📈 Analytics
The application treats your progress like an athlete's performance data.

1. **Trend Charts:** Navigate to the **Statistics** window to see your accuracy trend over time.
2. **Character Analysis:** A bar chart identifies your "Problem Characters"—those with the highest error rates.
3. **Persistent Logs:** All data is saved in a `statistics.csv` file in the application folder. This allows you to track progress over months or even years.

---

## 🔌 Hardware Integrity
For a truly authentic experience, CW_trainer-GNR supports external hardware keyers.

* **Connection:** Plug your Arduino/ESP32 keyer into your USB port.
* **Integrity Check:** The app uses a **Serial Manager** to monitor the connection status in real-time. If the device is disconnected, the app will alert you.
* **Settings:** Ensure your hardware is set to **115200 Baud**.
* **TX Mapping:** In TX mode, the app decodes the incoming Serial characters and compares them against the target string to grade your manual sending.

---

## 📥 How to Download
You can find pre-built binaries in the **Download/** directory of this repository:

* **Windows:** Download the `.exe` for installation or `.zip` for portable use.
* **Linux:** Download the `.AppImage` for a "no-install" experience on any distribution.

---

## 💻 Platform Setup

### 🪟 Windows
1. **Installer Method:** Run `CW_Trainer-GNR_vX.X.X_Setup.exe`. It will create desktop shortcuts and an uninstaller.
2. **Portable Method:** Extract `CW_Trainer-GNR_vX.X.X_Portable.zip`. Run `CW_Trainer-GNR.exe` inside. 

### 🐧 Linux
1. Download the `.AppImage`.
2. Right-click the file -> **Properties** -> **Permissions** -> **Allow executing file as program**.
3. Alternatively, run in terminal: `chmod +x CW_Trainer-GNR*.AppImage && ./CW_Trainer-GNR*.AppImage`.

### 🍎 macOS
*Currently available via Source Build:*
1. Install **Qt 6** and **Qt Creator**.
2. **Open Project:** Open `CW_Trainer-GNR.pro`.
3. **Run:** Select the **Desktop Qt 6 (clang)** kit and press **Run**.

---

**Developed with ❤️ for the CW Community.**
