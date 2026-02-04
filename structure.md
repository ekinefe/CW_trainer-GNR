# Project Structure and Code Documentation

This document contains the complete structure and code usage for the CW_trainer-GNR project.

## Directory Structure

```
/run/media/ekinefegungor/Shared/CW_keyer/CW_trainer-GNR/
├── .git/
├── .gitignore
├── README.md
├── structure.md
└── src/
    ├── CheatSheetWindow.cpp
    ├── CheatSheetWindow.h
    ├── MainWindow.cpp
    ├── MainWindow.h
    ├── MorseUtils.h
    ├── SerialManager.cpp
    ├── SerialManager.h
    ├── SoundGenerator.cpp
    ├── SoundGenerator.h
    ├── StatisticsTracker.cpp
    ├── StatisticsTracker.h
    ├── StatisticsWindow.cpp
    ├── StatisticsWindow.h
    └── main.cpp
```

## File Contents

### `.gitignore`

```gitignore
# Prerequisites
*.d

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Precompiled Headers
*.gch
*.pch

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Fortran module files
*.mod
*.smod

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app
CW_trainer-GNR

# Build directories
build/
bin/
debug/
release/
x64/
x86/
obj/

# CMake
CMakeCache.txt
CMakeFiles/
Makefile
cmake_install.cmake
install_manifest.txt
*.cmake
!CMakeLists.txt

# Qt
*.pro.user
*.qmake.stash
.qmake.cache
ui_*.h
moc_*.cpp
qrc_*.cpp
Makefile*

# IDEs
.vscode/
.idea/
*.user
*.swp
*~

# OS
.DS_Store
Thumbs.db

# Sturucture and explanation of the project
structure.md
```

### `src/CheatSheetWindow.h`

```cpp
#ifndef CHEATSHEETWINDOW_H
#define CHEATSHEETWINDOW_H

#include <QDialog>
#include <QMap>

class CheatSheetWindow : public QDialog
{
    Q_OBJECT
public:
    explicit CheatSheetWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<QChar, QString> m_morseCode;
    void initMorseCode();
};

#endif // CHEATSHEETWINDOW_H
```

### `src/CheatSheetWindow.cpp`

```cpp
#include "CheatSheetWindow.h"
#include <QPainter>
#include <QGridLayout>
#include <QLabel>
#include "MorseUtils.h"

CheatSheetWindow::CheatSheetWindow(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Morse Code Cheat Sheet");
    resize(400, 600); // Set a reasonable default size

    // We can use a layout or just paint it. 
    // Painting is often nicer for custom grid looks, but URL/Labels are easier with layouts.
    // Let's use custom painting for a "Cheat Sheet" look as requested by the style.
    // Actually, simple painting is fine.
}

void CheatSheetWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.fillRect(rect(), QColor("#f0f0f0"));

    painter.setPen(Qt::black);
    painter.setFont(QFont("Courier New", 12, QFont::Bold));

    int xStart = 20;
    int yStart = 30;
    int colWidth = 120;
    int rowHeight = 25;
    
    int rowsPerCol = 20; 

    // Get the map
    const QMap<QChar, QString> &map = MorseUtils::getMorseMap();
    
    int count = 0;
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        int col = count / rowsPerCol;
        int row = count % rowsPerCol;

        int x = xStart + col * colWidth;
        int y = yStart + row * rowHeight;

        QString text = QString("%1 : %2").arg(it.key()).arg(it.value());
        painter.drawText(x, y, text);
        
        count++;
    }
}
```

### `src/MainWindow.h`

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include "SerialManager.h"
#include "SoundGenerator.h"
#include "StatisticsTracker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectBtnClicked();
    void onRefreshBtnClicked();
    void onPlayBtnClicked();
    void onStopBtnClicked();
    void onWpmChanged(int value);
    void onFreqChanged(int value);
    void onModeChanged(int index); // 0: Random, 1: Koch, 2: Echo, 3: Keying (TX)
    void onShowStatsClicked();
    void onShowCheatSheetClicked();
    
    // Serial/Morse handling
    void onSerialDataReceived(const QString &data);
    void processIncomingMorse(const QString &morseSequence);
    
    // Game/Practice Logic
    void generateNextCharacter();
    void provideFeedback(bool correct);
    void checkInput(const QString &input);
    void playCharacterAudio(QChar c);

    // TX Mode
    void handleTxInput(const QString &decodedChar);

private:
    void setupUi();
    void updateStatusBar(const QString &msg);
    
    // UI Elements
    QComboBox *m_portCombo;
    QPushButton *m_connectBtn;
    QPushButton *m_refreshBtn;
    
    QSlider *m_wpmSlider;
    QLabel *m_wpmLabel;
    QSlider *m_freqSlider;
    QLabel *m_freqLabel;
    
    QComboBox *m_modeCombo;
    
    QLabel *m_targetCharLabel; // Big display of char to type
    QLineEdit *m_inputBox;     // User typing area
    QLabel *m_feedbackLabel;   // Correct/Incorrect
    
    QPushButton *m_playBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_statsBtn;
    QPushButton *m_cheatBtn;

    QTextEdit *m_logArea;      // Debug/Serial log

    // Logic
    SerialManager *m_serial;
    SoundGenerator *m_soundGen;
    StatisticsTracker *m_stats;
    
    bool m_isPlaying;
    int m_wpm;
    int m_freq;
    int m_currentMode; // 0: Random, 1: Koch, 2: Echo, 3: TX
    
    QChar m_currentChar; // The character user needs to type (RX mode) or mimics (Echo)
    QString m_txBuffer;  // For TX mode accumulation
    
    // Koch sequence
    QString m_kochSequence = "KMRSUAPTLOWI.NJEF0Y,VG5/Q9ZH38B?427C1D6X";
    int m_kochLevel = 2; // Start with first 2 chars
};

#endif // MAINWINDOW_H
```

### `src/MainWindow.cpp`

```cpp
#include "MainWindow.h"
#include "StatisticsWindow.h"
#include "CheatSheetWindow.h"
#include "MorseUtils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDebug>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_serial(new SerialManager(this))
    , m_soundGen(new SoundGenerator(this))
    , m_stats(new StatisticsTracker(this))
    , m_isPlaying(false)
    , m_wpm(20)
    , m_freq(600)
    , m_currentMode(0)
    , m_kochLevel(2)
{
    setupUi();
    
    connect(m_serial, &SerialManager::dataReceived, this, &MainWindow::onSerialDataReceived);
    
    // Default Status
    updateStatusBar("Ready. Please connect to CW Keyer.");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // --- Connection Group ---
    QGroupBox *connGroup = new QGroupBox("Device Connection", this);
    QHBoxLayout *connLayout = new QHBoxLayout(connGroup);
    
    m_portCombo = new QComboBox(this);
    m_refreshBtn = new QPushButton("Refresh", this);
    m_connectBtn = new QPushButton("Connect", this);
    
    connLayout->addWidget(new QLabel("Port:"));
    connLayout->addWidget(m_portCombo);
    connLayout->addWidget(m_refreshBtn);
    connLayout->addWidget(m_connectBtn);
    
    onRefreshBtnClicked(); // Populate initially
    
    // --- Settings Group ---
    QGroupBox *settingsGroup = new QGroupBox("Settings", this);
    QGridLayout *settLayout = new QGridLayout(settingsGroup);
    
    m_wpmSlider = new QSlider(Qt::Horizontal, this);
    m_wpmSlider->setRange(5, 50);
    m_wpmSlider->setValue(m_wpm);
    m_wpmLabel = new QLabel(QString("WPM: %1").arg(m_wpm), this);
    
    m_freqSlider = new QSlider(Qt::Horizontal, this);
    m_freqSlider->setRange(300, 1200);
    m_freqSlider->setValue(m_freq);
    m_freqLabel = new QLabel(QString("Tone: %1 Hz").arg(m_freq), this);
    
    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItem("Random Characters");
    m_modeCombo->addItem("Koch Method");
    m_modeCombo->addItem("Echo Practice (Call & Response)");
    m_modeCombo->addItem("TX Practice (Keying)");
    
    settLayout->addWidget(m_wpmLabel, 0, 0);
    settLayout->addWidget(m_wpmSlider, 0, 1);
    settLayout->addWidget(m_freqLabel, 1, 0);
    settLayout->addWidget(m_freqSlider, 1, 1);
    settLayout->addWidget(new QLabel("Mode:"), 2, 0);
    settLayout->addWidget(m_modeCombo, 2, 1);
    
    // --- Practice Area ---
    QGroupBox *practiceGroup = new QGroupBox("Practice Arena", this);
    QVBoxLayout *pracLayout = new QVBoxLayout(practiceGroup);
    
    m_targetCharLabel = new QLabel("?", this);
    m_targetCharLabel->setAlignment(Qt::AlignCenter);
    QFont f = m_targetCharLabel->font();
    f.setPointSize(72);
    f.setBold(true);
    m_targetCharLabel->setFont(f);
    m_targetCharLabel->setStyleSheet("color: #007acc;"); // Blue-ish
    
    m_inputBox = new QLineEdit(this);
    m_inputBox->setPlaceholderText("Type character here...");
    m_inputBox->setAlignment(Qt::AlignCenter);
    QFont f2 = m_inputBox->font();
    f2.setPointSize(24);
    m_inputBox->setFont(f2);
    
    m_feedbackLabel = new QLabel("Get Ready!", this);
    m_feedbackLabel->setAlignment(Qt::AlignCenter);
    
    pracLayout->addWidget(m_targetCharLabel);
    pracLayout->addWidget(m_feedbackLabel);
    pracLayout->addWidget(m_inputBox);
    
    // --- Control Buttons ---
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_playBtn = new QPushButton("Start Practice", this);
    m_stopBtn = new QPushButton("Stop", this);
    m_stopBtn->setEnabled(false);
    m_statsBtn = new QPushButton("Statistics", this);
    m_cheatBtn = new QPushButton("Cheat Sheet", this);
    
    btnLayout->addWidget(m_playBtn);
    btnLayout->addWidget(m_stopBtn);
    btnLayout->addWidget(m_statsBtn);
    btnLayout->addWidget(m_cheatBtn);
    
    // --- Log Area ---
    m_logArea = new QTextEdit(this);
    m_logArea->setReadOnly(true);
    m_logArea->setMaximumHeight(100);
    
    // Add all to main
    mainLayout->addWidget(connGroup);
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(practiceGroup);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_logArea);
    
    // Connect Signals
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshBtnClicked);
    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectBtnClicked);
    connect(m_wpmSlider, &QSlider::valueChanged, this, &MainWindow::onWpmChanged);
    connect(m_freqSlider, &QSlider::valueChanged, this, &MainWindow::onFreqChanged);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);
    connect(m_playBtn, &QPushButton::clicked, this, &MainWindow::onPlayBtnClicked);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::onStopBtnClicked);
    connect(m_inputBox, &QLineEdit::textChanged, this, &MainWindow::checkInput);
    connect(m_statsBtn, &QPushButton::clicked, this, &MainWindow::onShowStatsClicked);
    connect(m_cheatBtn, &QPushButton::clicked, this, &MainWindow::onShowCheatSheetClicked);
}

void MainWindow::onRefreshBtnClicked()
{
    m_portCombo->clear();
    const auto ports = m_serial->availablePorts();
    for (const auto &p : ports) {
        m_portCombo->addItem(p);
    }
}

void MainWindow::onConnectBtnClicked()
{
    if (m_serial->isConnected()) {
        m_serial->disconnectDevice();
        m_connectBtn->setText("Connect");
        m_portCombo->setEnabled(true);
        updateStatusBar("Disconnected.");
    } else {
        QString portName = m_portCombo->currentText();
        if (m_serial->connectDevice(portName)) {
            m_connectBtn->setText("Disconnect");
            m_portCombo->setEnabled(false);
            updateStatusBar("Connected to " + portName);
        } else {
            QMessageBox::critical(this, "Error", "Could not connect to port.");
        }
    }
}

void MainWindow::onWpmChanged(int value)
{
    m_wpm = value;
    m_wpmLabel->setText(QString("WPM: %1").arg(m_wpm));
    // Ideally send command to Arduino to sync WPM if needed
}

void MainWindow::onFreqChanged(int value)
{
    m_freq = value;
    m_freqLabel->setText(QString("Tone: %1 Hz").arg(m_freq));
    m_soundGen->setFrequency(m_freq);
}

void MainWindow::onModeChanged(int index)
{
    m_currentMode = index;
    if (m_currentMode == 3) {
        // TX Mode
        m_targetCharLabel->setText("TX Mode");
        m_feedbackLabel->setText("Key with your paddle!");
        m_inputBox->setEnabled(false);
        m_inputBox->setText("");
    } else {
        m_inputBox->setEnabled(true);
        m_targetCharLabel->setText("?");
    }
}

void MainWindow::onPlayBtnClicked()
{
    m_isPlaying = true;
    m_playBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
    m_modeCombo->setEnabled(false);
    
    m_stats->startSession();
    
    m_inputBox->setFocus();
    m_inputBox->clear();
    
    generateNextCharacter();
}

void MainWindow::onStopBtnClicked()
{
    m_isPlaying = false;
    m_playBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
    m_modeCombo->setEnabled(true);
    m_stats->endSession();
    
    m_targetCharLabel->setText("?");
    m_feedbackLabel->setText("Stopped.");
}

void MainWindow::generateNextCharacter()
{
    if (!m_isPlaying) return;
    
    if (m_currentMode == 0) { // Random
        // A-Z 0-9
        const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        int idx = QRandomGenerator::global()->bound(chars.length());
        m_currentChar = chars[idx];
    } else if (m_currentMode == 1) { // Koch
        // Use subset based on level
        QString sub = m_kochSequence.left(m_kochLevel);
        int idx = QRandomGenerator::global()->bound(sub.length());
        m_currentChar = sub[idx];
    } else if (m_currentMode == 2) { // Echo
        // Same as Random for now, but behavior differs in check
         const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
         int idx = QRandomGenerator::global()->bound(chars.length());
         m_currentChar = chars[idx];
    } else if (m_currentMode == 3) { // TX
        // TX doesn't generate characters to receive, it waits for input
        // But for training, maybe we ask user to Key a char?
        // Let's implement: App shows char, User keys it.
        const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        int idx = QRandomGenerator::global()->bound(chars.length());
        m_currentChar = chars[idx];
    }
    
    m_targetCharLabel->setText(QString(m_currentChar));
    
    if (m_currentMode != 3) {
        // Play Audio for RX modes
        playCharacterAudio(m_currentChar);
    }
    
    // Clear input for next attempt
    m_inputBox->clear();
}

void MainWindow::playCharacterAudio(QChar c)
{
    QString code = MorseUtils::charToMorse(c);
    // Beep it (async)
    m_soundGen->playMorse(code, m_wpm);
}

void MainWindow::checkInput(const QString &input)
{
    if (!m_isPlaying) return;
    if (input.isEmpty()) return;
    
    QChar lastChar = input.toUpper().at(input.length() - 1);
    
    // We only check single char matches immediately for fast practice
    if (lastChar == m_currentChar) {
        provideFeedback(true);
        // Delay slightly then next
        QTimer::singleShot(500, this, &MainWindow::generateNextCharacter);
    } else if (input.length() >= 1) {
        // Incorrect
        // If they typed more than 1 char, reset
        provideFeedback(false);
        m_inputBox->clear(); // Force retry
    }
}

void MainWindow::provideFeedback(bool correct)
{
    m_stats->recordAttempt(m_currentChar, correct);
    
    if (correct) {
        m_feedbackLabel->setText("Correct!");
        m_feedbackLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        m_feedbackLabel->setText("Try Again!");
        m_feedbackLabel->setStyleSheet("color: red; font-weight: bold;");
    }
}

void MainWindow::onSerialDataReceived(const QString &data)
{
    // Log
    m_logArea->append("RX: " + data);
    
    // If we are in TX Mode (Mode 3), we treat serial data as user input
    // The device sends decoded characters or raw text
    // Assuming device sends "T" or "E" or "Encoded: ..."
    // We look for single chars or content AFTER "Encoded:"
    
    // Simple parser: check if data contains "Encoded:"
    if (m_currentMode == 3) {
        QString clean = data.trimmed();
        if (clean.startsWith("Encoded:")) {
            QString content = clean.mid(8).trimmed();
            handleTxInput(content);
        } else {
            // Assume raw char
             handleTxInput(clean);
        }
    }
}

void MainWindow::handleTxInput(const QString &decodedChar)
{
    if (!m_isPlaying) return;
    
    // decodedChar might be a string like "ABC"
    // Check against m_currentChar
    // Just check the last char if it matches
    for (QChar c : decodedChar) {
        if (c.toUpper() == m_currentChar) {
            provideFeedback(true);
            QTimer::singleShot(200, this, &MainWindow::generateNextCharacter);
            return; 
        }
    }
}

void MainWindow::onShowStatsClicked()
{
    StatisticsWindow *w = new StatisticsWindow(m_stats, this);
    w->exec();
}

void MainWindow::onShowCheatSheetClicked()
{
    CheatSheetWindow *w = new CheatSheetWindow(this);
    w->exec();
}

void MainWindow::updateStatusBar(const QString &msg)
{
    statusBar()->showMessage(msg);
}
```

### `src/MorseUtils.h`

```cpp
#ifndef MORSEUTILS_H
#define MORSEUTILS_H

#include <QString>
#include <QMap>
#include <QChar>

class MorseUtils
{
public:
    static QString charToMorse(QChar c) {
        static QMap<QChar, QString> map;
        if (map.isEmpty()) {
            map.insert('A', ".-");
            map.insert('B', "-...");
            map.insert('C', "-.-.");
            map.insert('D', "-..");
            map.insert('E', ".");
            map.insert('F', "..-.");
            map.insert('G', "--.");
            map.insert('H', "....");
            map.insert('I', "..");
            map.insert('J', ".---");
            map.insert('K', "-.-");
            map.insert('L', ".-..");
            map.insert('M', "--");
            map.insert('N', "-.");
            map.insert('O', "---");
            map.insert('P', ".--.");
            map.insert('Q', "--.-");
            map.insert('R', ".-.");
            map.insert('S', "...");
            map.insert('T', "-");
            map.insert('U', "..-");
            map.insert('V', "...-");
            map.insert('W', ".--");
            map.insert('X', "-..-");
            map.insert('Y', "-.--");
            map.insert('Z', "--..");
            map.insert('0', "-----");
            map.insert('1', ".----");
            map.insert('2', "..---");
            map.insert('3', "...--");
            map.insert('4', "....-");
            map.insert('5', ".....");
            map.insert('6', "-....");
            map.insert('7', "--...");
            map.insert('8', "---..");
            map.insert('9', "----.");
            map.insert('.', ".-.-.-");
            map.insert(',', "--..--");
            map.insert('?', "..--..");
            map.insert('/', "-..-.");
            map.insert('@', ".--.-.");
        }
        return map.value(c.toUpper(), "");
    }
    
    static const QMap<QChar, QString>& getMorseMap() {
        // Just call charToMorse with dummy to ensure init, or make init separate.
        // Quick hack:
        charToMorse('A');
        
        static QMap<QChar, QString> map;
        if (map.isEmpty()) {
             // Re-populate access
             QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,?/@";
             for (QChar c : chars) {
                 map.insert(c, charToMorse(c));
             }
        }
        return map;
    }
};

#endif // MORSEUTILS_H
```

### `src/SerialManager.h`

```cpp
#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = nullptr);
    ~SerialManager();

    QStringList availablePorts();
    bool connectDevice(const QString &portName);
    void disconnectDevice();
    bool isConnected() const;
    
    void sendData(const QString &data);

signals:
    void dataReceived(const QString &data);
    void deviceConnected();
    void deviceDisconnected();
    void errorOccurred(const QString &msg);

private slots:
    void onReadyRead();

private:
    QSerialPort *m_serial;
    bool m_connected;
};

#endif // SERIALMANAGER_H
```

### `src/SerialManager.cpp`

```cpp
#include "SerialManager.h"
#include <QDebug>

SerialManager::SerialManager(QObject *parent) 
    : QObject(parent), m_connected(false)
{
    m_serial = new QSerialPort(this);
    
    connect(m_serial, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
}

SerialManager::~SerialManager()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}

QStringList SerialManager::availablePorts()
{
    QStringList ports;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPortInfos) {
        ports.append(info.portName());
    }
    return ports;
}

bool SerialManager::connectDevice(const QString &portName)
{
    if (m_serial->isOpen()) m_serial->close();
    
    m_serial->setPortName(portName);
    m_serial->setBaudRate(QSerialPort::Baud115200); // Standard for ESP32/Arduino
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_connected = true;
        emit deviceConnected();
        return true;
    }
    
    m_connected = false;
    emit errorOccurred(m_serial->errorString());
    return false;
}

void SerialManager::disconnectDevice()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
    m_connected = false;
    emit deviceDisconnected();
}

bool SerialManager::isConnected() const
{
    return m_connected;
}

void SerialManager::sendData(const QString &data)
{
    if (m_connected && m_serial->isOpen()) {
        m_serial->write(data.toUtf8());
    }
}

void SerialManager::onReadyRead()
{
    QByteArray data = m_serial->readAll();
    QString str = QString::fromUtf8(data);
    emit dataReceived(str);
}
```

### `src/SoundGenerator.h`

```cpp
#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H

#include <QObject>
#include <QAudioOutput>
#include <QAudioSink>
#include <QByteArray>
#include <QBuffer>

class SoundGenerator : public QObject
{
    Q_OBJECT
public:
    explicit SoundGenerator(QObject *parent = nullptr);
    ~SoundGenerator();

    void setFrequency(int hz);
    void playMorse(const QString &morseCode, int wpm);
    void stop();

private:
    void generateTone(int durationMs);
    void generateSilence(int durationMs);
    
    // Simple Qt Multimedia audio generation
    // (In Qt6, use QAudioSink. Qt5 uses QAudioOutput differently)
    // Assuming Qt6 for simplicity, or we mock it.
    
    // Since implementing a full PCM generator here is verbose, 
    // we'll implement a simplified blocking or queued sound queue.
    
    int m_frequency;
    // ... Audio members
};

#endif // SOUNDGENERATOR_H
```

### `src/SoundGenerator.cpp`

```cpp
#include "SoundGenerator.h"
#include <QtMath>
#include <QThread>

SoundGenerator::SoundGenerator(QObject *parent) : QObject(parent), m_frequency(600)
{
    // Setup Audio Output (simplified)
}

SoundGenerator::~SoundGenerator()
{
}

void SoundGenerator::setFrequency(int hz)
{
    m_frequency = hz;
}

void SoundGenerator::playMorse(const QString &morseCode, int wpm)
{
    // Timing calculations (Paris standard)
    // Unit length = 1200 / wpm (ms)
    
    int unitMs = 1200 / wpm;
    
    for (QChar c : morseCode) {
        if (c == '.') {
            generateTone(unitMs);
        } else if (c == '-') {
            generateTone(unitMs * 3);
        }
        // Intra-character gap
        generateSilence(unitMs);
    }
    // Inter-character gap is 3 units, but we already added 1 unit gap above.
    // So add 2 more units. 
    generateSilence(unitMs * 2); 
    
    // NOTE: This implementation is blocking for simplicity of example!
    // In real App, use a separate thread or async buffer filling.
}

void SoundGenerator::generateTone(int durationMs)
{
    // Placeholder for actual Beep
    // On Linux/Windows we can use system beep or Qt Audio
    // For now, logging
    // qDebug() << "BEEP" << durationMs << "ms";
    QThread::msleep(durationMs);
}

void SoundGenerator::generateSilence(int durationMs)
{
    // qDebug() << "..." << durationMs << "ms";
    QThread::msleep(durationMs);
}

void SoundGenerator::stop()
{
    // Stop playing
}
```

### `src/StatisticsTracker.h`

```cpp
#ifndef STATISTICSTRACKER_H
#define STATISTICSTRACKER_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QFile>

struct CharStats {
    int given;
    int correct;
    int wrong;
};

class StatisticsTracker : public QObject
{
    Q_OBJECT
public:
    explicit StatisticsTracker(QObject *parent = nullptr);
    
    void startSession();
    void endSession();
    
    void recordAttempt(QChar c, bool isCorrect);
    
    QMap<QChar, CharStats> getCharStats() const;
    int getTotalAttempts() const;
    int getCorrectCount() const;
    int getCurrentWpm() const; // Estimated
    QDateTime getStartTime() const;

private:
    void saveSessionToCsv(); // Append to statistics.csv

    bool m_inSession;
    QDateTime m_startTime;
    QMap<QChar, CharStats> m_stats;
    int m_totalAttempts;
    int m_totalCorrect;
};

#endif // STATISTICSTRACKER_H
```

### `src/StatisticsTracker.cpp`

```cpp
#include "StatisticsTracker.h"
#include <QTextStream>
#include <QDir>
#include <QDebug>

StatisticsTracker::StatisticsTracker(QObject *parent) : QObject(parent)
{
    m_inSession = false;
    m_totalAttempts = 0;
    m_totalCorrect = 0;
}

void StatisticsTracker::startSession()
{
    m_inSession = true;
    m_startTime = QDateTime::currentDateTime();
    m_stats.clear();
    m_totalAttempts = 0;
    m_totalCorrect = 0;
}

void StatisticsTracker::endSession()
{
    if (m_inSession) {
        saveSessionToCsv();
    }
    m_inSession = false;
}

void StatisticsTracker::recordAttempt(QChar c, bool isCorrect)
{
    if (!m_inSession) return;
    
    c = c.toUpper();
    m_stats[c].given++;
    if (isCorrect) {
        m_stats[c].correct++;
        m_totalCorrect++;
    } else {
        m_stats[c].wrong++;
    }
    m_totalAttempts++;
}

QMap<QChar, CharStats> StatisticsTracker::getCharStats() const
{
    return m_stats;
}

int StatisticsTracker::getTotalAttempts() const
{
    return m_totalAttempts;
}

int StatisticsTracker::getCorrectCount() const
{
    return m_totalCorrect;
}

int StatisticsTracker::getCurrentWpm() const
{
    // Stub
    return 20;
}

QDateTime StatisticsTracker::getStartTime() const
{
    return m_startTime;
}

void StatisticsTracker::saveSessionToCsv()
{
    QFile file("statistics.csv");
    bool isNew = !file.exists();
    
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        
        // 1. Header if new
        // We need a robust CSV schema. 
        // Date,Time,Duration,WPM,Accuracy, [A_Total,A_Err, ..., 0_Total, 0_Err]
        QList<QChar> allChars;
        // Collect all possible keys we track
        QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,?/@";
        for (QChar c : chars) allChars.append(c);
        
        if (isNew) {
            out << "Date,Time,Duration,WPM,Accuracy";
            for (QChar c : allChars) {
                QString safec = (c == ',') ? "COMMA" : QString(c);
                out << "," << safec << "_Total," << safec << "_Err";
            }
            out << "\n";
        }
        
        // 2. Data Line
        QDateTime now = QDateTime::currentDateTime();
        qint64 duration = m_startTime.secsTo(now);
        double accuracy = (m_totalAttempts > 0) ? ((double)m_totalCorrect / m_totalAttempts * 100.0) : 0.0;
        
        out << m_startTime.toString("yyyy-MM-dd") << ","
            << m_startTime.toString("HH:mm:ss") << ","
            << duration << ","
            << getCurrentWpm() << ","
            << QString::number(accuracy, 'f', 1);
            
        for (QChar c : allChars) {
            int total = m_stats.value(c).given;
            int err = m_stats.value(c).wrong;
            out << "," << total << "," << err;
        }
        out << "\n";
        
        file.close();
    }
}
```

### `src/StatisticsWindow.cpp`

```cpp
#include "StatisticsWindow.h"
#include "StatisticsTracker.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <algorithm>
#include <QPen>
#include <QBrush>

// --- TrendChart Implementation ---

TrendChart::TrendChart(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(200);
}

void TrendChart::setData(const QList<SessionData> &data)
{
    m_data = data;
    update();
}

void TrendChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), Qt::white);
    
    if (m_data.isEmpty()) {
        painter.drawText(rect(), Qt::AlignCenter, "No Session Data Available");
        return;
    }
    
    // Margins
    int mL = 40, mR = 20, mT = 20, mB = 30;
    int w = width() - mL - mR;
    int h = height() - mT - mB;
    
    // Axes
    painter.drawLine(mL, height() - mB, width() - mR, height() - mB); // X
    painter.drawLine(mL, height() - mB, mL, mT); // Y
    
    // Scale Y (0-100 Accuracy)
    // Scale X (Sessions)
    
    int count = m_data.size();
    double xStep = (double)w / (count > 1 ? count - 1 : 1);
    
    QPointF lastPoint;
    
    painter.setPen(QPen(Qt::blue, 2));
    
    for (int i = 0; i < count; ++i) {
        double acc = m_data[i].accuracy; // 0-100
        
        double x = mL + i * xStep;
        double y = (height() - mB) - (acc / 100.0 * h);
        
        QPointF p(x, y);
        
        if (i > 0) {
            painter.drawLine(lastPoint, p);
        }
        painter.drawEllipse(p, 3, 3);
        lastPoint = p;
    }
    
    painter.setPen(Qt::black);
    painter.drawText(mL, mT - 5, "Accuracy History");
}

// --- ProblemCharChart Implementation ---

ProblemCharChart::ProblemCharChart(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(150);
}

void ProblemCharChart::setData(const QList<CharErrorData> &data)
{
    m_data = data;
    update();
}

void ProblemCharChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);
    
    if (m_data.isEmpty()) {
        painter.drawText(rect(), Qt::AlignCenter, "No Errors Recorded");
        return;
    }
    
    int padL = 60;
    int padR = 20;
    int rowH = height() / (m_data.size() + 1);
    int barH = rowH - 10;
    if (barH < 5) barH = 5;
    
    int plotW = width() - padL - padR;
    
    painter.setPen(Qt::black);
    painter.drawText(10, 20, "Problem Chars (Top 5)");
    
    for (int i = 0; i < m_data.size(); ++i) {
        int yTop = 30 + i * rowH;
        
        // Label
        QString lbl = QString("%1 (%2)").arg(m_data[i].character).arg(m_data[i].totalAttempts);
        painter.drawText(5, yTop + barH/2 + 5, lbl);
        
        // Bar (Red)
        double barLen = (m_data[i].errorRate / 100.0) * plotW;
        double actualBarH = barH; 
        painter.fillRect(QRectF(padL, yTop, barLen, actualBarH), QColor("#dc3545"));

        // Value Label
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 8));
        painter.drawText(QRectF(padL + barLen + 5, yTop, 50, actualBarH), Qt::AlignLeft | Qt::AlignVCenter, QString::number(m_data[i].errorRate, 'f', 1) + "%");
    }
}

// --- StatisticsWindow Implementation ---

StatisticsWindow::StatisticsWindow(StatisticsTracker *tracker, QWidget *parent) 
    : QDialog(parent), m_tracker(tracker)
{
    setWindowTitle("Session Statistics");
    resize(600, 500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    m_trendChart = new TrendChart(this);
    m_problemChart = new ProblemCharChart(this);
    
    mainLayout->addWidget(m_trendChart, 2); // 2/3 height
    mainLayout->addWidget(m_problemChart, 1); // 1/3 height
    
    // Load Data immediately
    refreshData();
}

void StatisticsWindow::refreshData()
{
    loadCsvData();
    // Merge live data on top
    mergeLiveData();
    
    m_trendChart->setData(m_sessions);
    m_problemChart->setData(m_problemChars);
}

void StatisticsWindow::loadCsvData()
{
    m_sessions.clear();
    m_problemChars.clear();
    
    QFile file("statistics.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    
    QTextStream in(&file);
    QString headerLine = in.readLine();
    QStringList headers = headerLine.split(",");
    
    int idxDate = -1; // Date column is sometimes separate or we assume ISO format
    int idxTime = -1; // Time column
    // Actually the code writes Date,Time,Duration...
    // Let's find indices dynamically
    int idxAcc = headers.indexOf("Accuracy");
    int idxWpm = headers.indexOf("WPM");
    
    // Map for char stats: char -> {idxTotal, idxErr}
    QMap<QString, QPair<int, int>> charCols;
    
    for (int i = 0; i < headers.size(); ++i) {
        QString h = headers[i];
        if (h.endsWith("_Total")) {
            QString c = h.left(h.length() - 6);
            if (c == "COMMA") c = ",";
            if (!charCols.contains(c)) charCols[c] = qMakePair(-1, -1);
            charCols[c].first = i;
        } else if (h.endsWith("_Err")) {
             QString c = h.left(h.length() - 4);
             if (c == "COMMA") c = ",";
             if (!charCols.contains(c)) charCols[c] = qMakePair(-1, -1);
             charCols[c].second = i;
        }
    }
    
    // Map to accumulate global char stats
    // Map to accumulate global char stats
    m_globalCharStats.clear(); // Reset global stats
    QMap<QChar, QPair<int, int>> globalCharStats; // Local var for accumulation before transferring to member


    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (parts.size() < headers.size()) continue; // Skip malformed lines
        
        // Parse Session
        SessionData s;
        // Parse Date/Time (cols 0 and 1)
        // Assume format yyyy-MM-dd HH:mm:ss
        QString dtStr = parts[0] + " " + parts[1];
        s.dateTime = QDateTime::fromString(dtStr, "yyyy-MM-dd HH:mm:ss");
        
        bool ok;
        if (idxWpm != -1) s.wpm = parts[idxWpm].toInt(&ok); else s.wpm = 0;
        if (idxAcc != -1) s.accuracy = parts[idxAcc].toDouble(&ok); else s.accuracy = 0;
        
        m_sessions.append(s);
        
        // Accumulate Char Stats
        auto it = charCols.constBegin();
        while (it != charCols.constEnd()) {
            QChar c = it.key()[0];
            int idxT = it.value().first;
            int idxE = it.value().second;
            
            if (idxT != -1 && idxE != -1 && idxT < parts.size() && idxE < parts.size()) {
                int t = parts[idxT].toInt();
                int e = parts[idxE].toInt();
                
                if (!globalCharStats.contains(c)) globalCharStats[c] = qMakePair(0, 0);
                globalCharStats[c].first += t;
                globalCharStats[c].second += e;
            }
            ++it;
        }
    }
    file.close();
    
    // Store accumulated CSV stats into member
    m_globalCharStats = globalCharStats;
}

void StatisticsWindow::mergeLiveData()
{
    if (!m_tracker) return;
    
    // 1. Add Current Session to Session List (Trend Chart)
    int attempts = m_tracker->getTotalAttempts();
    if (attempts > 0) {
        SessionData s;
        s.dateTime = m_tracker->getStartTime();
        s.wpm = m_tracker->getCurrentWpm();
        
        int correct = m_tracker->getCorrectCount();
        s.accuracy = (double)correct / attempts * 100.0;
        s.duration = s.dateTime.secsTo(QDateTime::currentDateTime());
        
        m_sessions.append(s);
    }
    
    // 2. Merge Character Stats (Problem Chart)
    QMap<QChar, CharStats> liveStats = m_tracker->getCharStats();
    
    for (auto it = liveStats.begin(); it != liveStats.end(); ++it) {
        QChar c = it.key();
        int total = it.value().given;
        int err = it.value().wrong;
        
        if (!m_globalCharStats.contains(c)) m_globalCharStats[c] = qMakePair(0, 0);
        m_globalCharStats[c].first += total;
        m_globalCharStats[c].second += err;
    }
    
    // Re-calculate problem chars from merged data
    m_problemChars.clear();
    for (auto it = m_globalCharStats.constBegin(); it != m_globalCharStats.constEnd(); ++it) {
        int total = it.value().first;
        int err = it.value().second;
        if (total > 0 && err > 0) { // Only count if there are errors
            CharErrorData data;
            data.character = it.key();
            data.totalAttempts = total;
            data.errorRate = (double)err / total * 100.0;
            m_problemChars.append(data);
        }
    }
    
    // Sort
    std::sort(m_problemChars.begin(), m_problemChars.end(), [](const CharErrorData &a, const CharErrorData &b) {
        return a.errorRate > b.errorRate;
    });
    
    // Take top 5
    if (m_problemChars.size() > 5) {
        m_problemChars = m_problemChars.mid(0, 5);
    }
}
```

### `src/StatisticsWindow.h`

```cpp
#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QList>
#include <QDateTime>
#include <QMap>

// Structure to hold data for a single session
struct SessionData {
    QDateTime dateTime;
    int wpm;
    double accuracy;
    int duration;
};

// Structure to hold error rate for a character
struct CharErrorData {
    QChar character;
    double errorRate; // 0.0 to 100.0
    int totalAttempts;
};

// Widget for drawing the Trend Chart (WPM and Accuracy)
class TrendChart : public QWidget
{
    Q_OBJECT
public:
    explicit TrendChart(QWidget *parent = nullptr);
    void setData(const QList<SessionData> &data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QList<SessionData> m_data;
};

// Widget for drawing the Problem Characters Bar Chart
class ProblemCharChart : public QWidget
{
    Q_OBJECT
public:
    explicit ProblemCharChart(QWidget *parent = nullptr);
    void setData(const QList<CharErrorData> &data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QList<CharErrorData> m_data;
};

// Main Window for Statistics
class StatisticsTracker; // Forward Declaration

class StatisticsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit StatisticsWindow(StatisticsTracker *tracker, QWidget *parent = nullptr);
    void refreshData(); // Reloads data from CSV AND current live session

private:
    void loadCsvData();
    void mergeLiveData(); // Helper to merge live stats
    
    TrendChart *m_trendChart;
    ProblemCharChart *m_problemChart;
    
    QList<SessionData> m_sessions;
    QList<CharErrorData> m_problemChars;
    
    // Internal accumulator for all stats (historical + live)
    QMap<QChar, QPair<int, int>> m_globalCharStats; // <Total, Err>
    
    StatisticsTracker *m_tracker; // Reference to live tracker
};

#endif // STATISTICSWINDOW_H
```

### `src/main.cpp`

```cpp
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
```
