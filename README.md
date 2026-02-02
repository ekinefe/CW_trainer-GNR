# CW_trainer-GNR

**CW_trainer-GNR** is a cross-platform desktop application built with **C++ and the Qt Framework**. It serves as a comprehensive Morse Code (CW) training station, featuring algorithmic learning modes, statistical analysis, and hardware integration via Serial/USB for external keyers.

## 🚀 Features

### 1. Training Modes
The application implements a central logic engine (`MainWindow.cpp`) that supports four distinct practice modes:
* **Random Characters:** Generates random alphanumeric strings (A-Z, 0-9) for standard decoding practice.
* **Koch Method:** Implements the Koch method for progressive learning, starting with a specific subset of characters (default Level 2: `K`, `M`).
* **Echo Practice:** A "Call & Response" mode where the user listens to a character and must mimic it.
* **TX (Transmit) Mode:** Disables internal generation and waits for input from an external physical keyer to verify sending accuracy.

### 2. Audio & Signal Processing
* **Synthesis:** Uses `SoundGenerator` to synthesize sine wave tones in real-time based on the Paris standard timing.
* **Adjustable Parameters:**
    * **WPM (Speed):** 5 to 50 Words Per Minute.
    * **Frequency (Pitch):** 300Hz to 1200Hz.

### 3. Analytics & Feedback
* **Real-time Feedback:** Immediate visual cues (Green/Red) upon typing a character.
* **Long-term Tracking (`StatisticsTracker`):**
    * Saves session data (Date, Time, Duration, Accuracy, WPM) to a persistent `statistics.csv` file.
    * Tracks accuracy for every individual character including alphanumeric and punctuation.
* **Visualizations (`StatisticsWindow`):**
    * **Trend Chart:** Plots accuracy over time across different sessions.
    * **Problem Areas:** A dynamic bar chart identifying the top 5 characters with the highest error rates.

### 4. Hardware Integration
* **Serial Manager:** Detects and connects to external hardware (e.g., Arduino/ESP32) via USB Serial.
* **Hot-Plugging:** Supports refreshing available ports dynamically.

---

## 🛠 Technical Requirements

### Dependencies
This project requires **Qt 6** (or Qt 5.15+) with the following modules enabled in the `.pro` file:
`QT += core gui widgets serialport multimedia`

### Build System
The project is structured for **qmake**, indicated by the presence of `.qmake.stash` and `.pro` user configurations.

---

## 🔌 Hardware Interface Protocol

To use the **TX Mode**, connect an external device (Arduino/STM32/ESP32) that decodes paddle input and sends it to the PC.

**Serial Settings:**
* **Baud Rate:** 115200
* **Data Bits:** 8
* **Parity:** None
* **Stop Bits:** 1

**Communication Protocol:**
The application parses incoming serial data in `MainWindow::onSerialDataReceived`. Your device should send data in one of two formats:
1.  **Raw Characters:** Simply print the decoded character (e.g., `A`).
2.  **Encoded Prefix:** Print the string `Encoded: ` followed by the content (e.g., `Encoded: SOS`).

---

## 📂 Project Structure

| File | Description |
| :--- | :--- |
| **src/MainWindow** | The central controller. Manages UI, logic components, and the main game loop. |
| **src/SerialManager** | Wrapper around `QSerialPort` for handling device connection and data buffering. |
| **src/SoundGenerator** | Manages audio output to generate Morse beep timings (dots, dashes, and silence). |
| **src/StatisticsTracker** | The "Database" logic. Accumulates session data and manages `statistics.csv`. |
| **src/StatisticsWindow** | Custom UI widget that draws graphs using `QPainter` to visualize session data. |
| **src/MorseUtils** | Static helper class for mapping characters to Morse strings (e.g., 'A' -> ".-"). |
| **src/CheatSheetWindow** | A dialog that renders the full Morse alphabet map for reference. |

---

## 💿 Build & Run Instructions

1.  **Clone the Repository**
2.  **Open in Qt Creator:** Select the `.pro` file.
3.  **Configure Kit:** Ensure a Desktop Kit (GCC, MSVC, or Clang) is selected.
4.  **Build:** Run `qmake` followed by `make` (or use the Build button in Qt Creator).
5.  **Run:** The executable `CW_trainer-GNR` will be generated in the build directory.

---

## 📊 Data Storage

Your progress is stored locally in `statistics.csv` in the application's working directory.
* **Format:** `Date, Time, Duration, WPM, Accuracy, [Char_Total, Char_Err]...`
* This file allows the application to reload historical accuracy and "Problem Character" charts upon restart.