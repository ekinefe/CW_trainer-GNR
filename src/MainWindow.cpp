#include "MainWindow.h"
#include "MorseUtils.h"
#include <QMessageBox>
#include <QTimer>
#include <QButtonGroup>
#include <QRandomGenerator>
#include <QDebug>

// Constructor for MainWindow
// Initializes the base class and component classes (Serial, Sound, Tracker)
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), 
      m_serial(new SerialManager(this)), 
      m_sound(new SoundGenerator(this)),
      m_tracker(new StatisticsTracker())
{
    // Build the UI
    setupUi();
    // Connect Signals and Slots
    setupConnections();
    // Populate the ports list
    refreshPorts();
    
    // Populate Audio Devices
    const auto devices = QMediaDevices::audioOutputs();
    for (const auto &device : devices) {
        m_comboAudioDevice->addItem(device.description(), QVariant::fromValue(device));
    }
    // Select default
    m_comboAudioDevice->setCurrentIndex(0); // Simplification: Just pick first (usually default)
    
    // Initialize Drill Timer
    m_drillTimer = new QTimer(this);
    m_drillTimer->setSingleShot(true);
    connect(m_drillTimer, &QTimer::timeout, this, &MainWindow::sendNextDrillChar);

    // Initial state
    toggleOfflineUi();
    toggleDrillUi();
    toggleSpacingUi();
}

// Destructor
// Clean up manually allocated non-child objects
MainWindow::~MainWindow()
{
    delete m_tracker; // StatisticsTracker is not a QObject child, so delete manually
}

// Handle Window Close Event
// Saves statistics before the application exits
void MainWindow::closeEvent(QCloseEvent *event)
{
    // Gather current session parameters for stats
    int wpm = m_spinOfflineWpm->value();
    int tone = m_spinOfflineTone->value();
    QString mode = m_chkOffline->isChecked() ? "OFFLINE" : "DEVICE";
    
    // Save the session data
    m_tracker->saveSession(wpm, tone, mode);
    
    // Accept the event to allow closing
    event->accept();
}

// Build the User Interface
void MainWindow::setupUi()
{
    // central widget setup
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    // --- Connection Card Setup ---
    QGroupBox *connBox = new QGroupBox("Connection");
    QHBoxLayout *connLayout = new QHBoxLayout(connBox);
    
    // Port Selection
    connLayout->addWidget(new QLabel("Port:"));
    m_portCombo = new QComboBox();
    connLayout->addWidget(m_portCombo);
    
    // Baud Rate Selection
    connLayout->addWidget(new QLabel("Baud:"));
    m_baudCombo = new QComboBox();
    m_baudCombo->addItems({"9600", "115200"}); // Common Baud Rates
    connLayout->addWidget(m_baudCombo);
    
    // Refresh Button
    QPushButton *btnRefresh = new QPushButton("Refresh");
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    connLayout->addWidget(btnRefresh);
    
    // Connect Button
    m_btnConnect = new QPushButton("Connect");
    connLayout->addWidget(m_btnConnect);
    
    // Status Label
    m_lblStatus = new QLabel("OFFLINE");
    m_lblStatus->setStyleSheet("color: red; font-weight: bold;"); // Red for offline
    connLayout->addWidget(m_lblStatus);
    
    mainLayout->addWidget(connBox);
    
    // --- Tabs Setup ---
    m_tabs = new QTabWidget();
    mainLayout->addWidget(m_tabs);
    
    // Dashboard Tab
    QWidget *dashTab = new QWidget();
    setupDashboard(dashTab);
    m_tabs->addTab(dashTab, "Dashboard");
    
    // Trainer Tab
    QWidget *trainerTab = new QWidget();
    setupTrainer(trainerTab);
    m_tabs->addTab(trainerTab, "CW Trainer");
}

// Setup Dashboard Tab Components
void MainWindow::setupDashboard(QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);

    // Device Status GroupBox
    QGroupBox *statusBox = new QGroupBox("Device Status");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusBox);
    
    // Status Labels
    m_lblDashWpm = new QLabel("Speed: -- WPM");
    m_lblDashTone = new QLabel("Tone: -- Hz");
    m_lblDashMode = new QLabel("Mode: --");
    
    statusLayout->addWidget(m_lblDashWpm);
    statusLayout->addWidget(m_lblDashTone);
    statusLayout->addWidget(m_lblDashMode);
    statusLayout->addWidget(new QLabel("(Knobs on Device)"));
    statusLayout->addStretch();
    
    // Checkbox for System Messages
    m_chkShowSys = new QCheckBox("Show System Msgs");
    statusLayout->addWidget(m_chkShowSys);
    
    // Clear LCD Button
    QPushButton *btnClearLcd = new QPushButton("Clear LCD");
    connect(btnClearLcd, &QPushButton::clicked, this, &MainWindow::sendSerialCommand); // Functionality reused/placeholder
    statusLayout->addWidget(btnClearLcd);
    
    // Cheat Sheet Button
    QPushButton *btnCheat = new QPushButton("Cheat Sheet");
    connect(btnCheat, &QPushButton::clicked, this, &MainWindow::toggleCheatSheet);
    statusLayout->addWidget(btnCheat);

    // Statistics Button
    QPushButton *btnStats = new QPushButton("Statistics");
    connect(btnStats, &QPushButton::clicked, this, &MainWindow::toggleStatistics);
    statusLayout->addWidget(btnStats);
    
    layout->addWidget(statusBox);
    
    // Terminal Area
    QHBoxLayout *termLayout = new QHBoxLayout();
    
    // RX (Receive) Area
    QGroupBox *rxBox = new QGroupBox("Inbox (RX from Paddle)");
    QVBoxLayout *rxLayout = new QVBoxLayout(rxBox);
    m_txtRx = new QTextEdit();
    m_txtRx->setReadOnly(true); // User cannot type here directly
    rxLayout->addWidget(m_txtRx);
    
    // Clear Log Button
    QPushButton *btnClearRx = new QPushButton("Clear Log");
    connect(btnClearRx, &QPushButton::clicked, this, &MainWindow::clearRxLog);
    rxLayout->addWidget(btnClearRx);
    termLayout->addWidget(rxBox);
    
    // TX (Transmit) Area
    QGroupBox *txBox = new QGroupBox("Outbox (TX via Serial)");
    QVBoxLayout *txLayout = new QVBoxLayout(txBox);
    m_txtTx = new QTextEdit();
    txLayout->addWidget(m_txtTx);
    
    // Send Button
    QPushButton *btnSend = new QPushButton("Send Text");
    connect(btnSend, &QPushButton::clicked, this, &MainWindow::sendSerialCommand);
    txLayout->addWidget(btnSend);
    termLayout->addWidget(txBox);
    
    layout->addLayout(termLayout);
}

// Setup Trainer Tab Components
void MainWindow::setupTrainer(QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);
    
    // --- Config Card ---
    QGroupBox *cfgBox = new QGroupBox("Training Configuration");
    QGridLayout *cfgLayout = new QGridLayout(cfgBox);
    
    // Mode Selection (Words vs Random)
    cfgLayout->addWidget(new QLabel("Mode:"), 0, 0);
    m_radioModeWords = new QRadioButton("Meaningful Words");
    m_radioModeRandom = new QRadioButton("Random Characters");
    m_radioModeWords->setChecked(true);
    
    // Group Buttons for logic exclusion
    QButtonGroup *grpMode = new QButtonGroup(this);
    grpMode->addButton(m_radioModeWords);
    grpMode->addButton(m_radioModeRandom);
    
    QHBoxLayout *modeLayout = new QHBoxLayout();
    modeLayout->addWidget(m_radioModeWords);
    modeLayout->addWidget(m_radioModeRandom);
    cfgLayout->addLayout(modeLayout, 0, 1, 1, 3);
    
    // Group and Set Size Inputs (For Random Mode)
    cfgLayout->addWidget(new QLabel("Group Size:"), 1, 0);
    m_spinGroupSize = new QSpinBox();
    m_spinGroupSize->setRange(1, 50);
    m_spinGroupSize->setValue(5); // Default group size 5
    cfgLayout->addWidget(m_spinGroupSize, 1, 1);
    
    cfgLayout->addWidget(new QLabel("Set Size:"), 1, 2);
    m_spinSetSize = new QSpinBox();
    m_spinSetSize->setRange(1, 10);
    m_spinSetSize->setValue(1);
    cfgLayout->addWidget(m_spinSetSize, 1, 3);
    
    // Drill Type Selection (RX vs TX)
    cfgLayout->addWidget(new QLabel("Drill Type:"), 2, 0);
    m_radioRx = new QRadioButton("RX (Receive Audio)");
    m_radioTx = new QRadioButton("TX (Keying)");
    m_radioRx->setChecked(true); // Default RX
    
    QButtonGroup *grpDrill = new QButtonGroup(this);
    grpDrill->addButton(m_radioRx);
    grpDrill->addButton(m_radioTx);
    // Connect toggle to update UI when drill type changes
    connect(m_radioRx, &QRadioButton::toggled, this, &MainWindow::toggleDrillUi);
    
    QHBoxLayout *drillLayout = new QHBoxLayout();
    drillLayout->addWidget(m_radioRx);
    drillLayout->addWidget(m_radioTx);
    cfgLayout->addLayout(drillLayout, 2, 1, 1, 3);
    
    // Allowed Characters Input
    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Allowed Chars:"));
    m_lineAllowedChars = new QLineEdit("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    filterLayout->addWidget(m_lineAllowedChars);
    cfgLayout->addLayout(filterLayout, 3, 0, 1, 4); // Span 4 columns
    
    // Spacing Checkbox
    m_chkIgnoreSpacing = new QCheckBox("Ignore Spacing");
    cfgLayout->addWidget(m_chkIgnoreSpacing, 4, 0, 1, 2);

    // Custom Spacing (Farnsworth)
    m_chkAdjustableSpacing = new QCheckBox("Adjustable Time Spacing");
    m_chkAdjustableSpacing->setToolTip("Add extra delay between characters (Farnsworth style)");
    cfgLayout->addWidget(m_chkAdjustableSpacing, 5, 0, 1, 2);
    
    m_spacingContainer = new QWidget();
    QHBoxLayout *spacingControls = new QHBoxLayout(m_spacingContainer);
    spacingControls->setContentsMargins(0,0,0,0);
    spacingControls->addWidget(new QLabel("Extra Delay (ms):"));
    m_spinSpacingMs = new QSpinBox();
    m_spinSpacingMs->setRange(0, 5000);
    m_spinSpacingMs->setValue(500); // Default 500ms
    m_spinSpacingMs->setSingleStep(50);
    spacingControls->addWidget(m_spinSpacingMs);
    cfgLayout->addWidget(m_spacingContainer, 5, 2, 1, 2);
    
    // Offline Checkbox and Controls
    m_chkOffline = new QCheckBox("Work Offline (PC Audio)");
    connect(m_chkOffline, &QCheckBox::toggled, this, &MainWindow::toggleOfflineUi);
    cfgLayout->addWidget(m_chkOffline, 6, 0, 1, 2);
    
    // Offline Controls (WPM/Tone) container
    m_offlineControlsInfo = new QWidget();
    QHBoxLayout *offLayout = new QHBoxLayout(m_offlineControlsInfo);
    offLayout->setContentsMargins(0,0,0,0);
    offLayout->addWidget(new QLabel("WPM:"));
    m_spinOfflineWpm = new QSpinBox();
    m_spinOfflineWpm->setRange(5, 60);
    m_spinOfflineWpm->setValue(20);
    offLayout->addWidget(m_spinOfflineWpm);
    
    offLayout->addWidget(new QLabel("Hz:"));
    m_spinOfflineTone = new QSpinBox();
    m_spinOfflineTone->setRange(400, 1200);
    m_spinOfflineTone->setValue(700);
    offLayout->addWidget(m_spinOfflineTone);
    
    cfgLayout->addWidget(m_offlineControlsInfo, 6, 2, 1, 2);
    
    // PC Audio Controls (Device + Volume)
    QGroupBox *audioBox = new QGroupBox("PC Audio Settings");
    QVBoxLayout *audioLayout = new QVBoxLayout(audioBox);
    
    // Device Selector
    audioLayout->addWidget(new QLabel("Output Device:"));
    m_comboAudioDevice = new QComboBox();
    audioLayout->addWidget(m_comboAudioDevice);
    
    // Volume Slider
    audioLayout->addWidget(new QLabel("Volume:"));
    m_sliderVolume = new QSlider(Qt::Horizontal);
    m_sliderVolume->setRange(0, 100);
    m_sliderVolume->setValue(100);
    audioLayout->addWidget(m_sliderVolume);
    
    cfgLayout->addWidget(audioBox, 7, 0, 1, 4);

    layout->addWidget(cfgBox);
    
    // --- Play Area ---
    QGroupBox *playBox = new QGroupBox("Practice Area");
    QVBoxLayout *playLayout = new QVBoxLayout(playBox);
    
    // Instructions Label
    m_lblInstruction = new QLabel("Press Play, Listen, Type Answer");
    m_lblInstruction->setAlignment(Qt::AlignCenter);
    playLayout->addWidget(m_lblInstruction);
    
    // Large Target Label (For TX mode)
    m_lblTargetBig = new QLabel("CQ DX");
    QFont f = m_lblTargetBig->font();
    f.setPointSize(28);
    f.setBold(true);
    m_lblTargetBig->setFont(f);
    m_lblTargetBig->setStyleSheet("color: #0055AA"); // Blue text
    m_lblTargetBig->setAlignment(Qt::AlignCenter);
    playLayout->addWidget(m_lblTargetBig);
    m_lblTargetBig->hide(); // Hide by default (RX mode)
    
    // Feedback Label
    m_lblFeedback = new QLabel("Ready");
    m_lblFeedback->setAlignment(Qt::AlignCenter);
    QFont f2 = m_lblFeedback->font();
    f2.setPointSize(14);
    f2.setBold(true);
    m_lblFeedback->setFont(f2);
    playLayout->addWidget(m_lblFeedback);
    
    // Answer Input
    m_entAnswer = new QLineEdit();
    QFont f3 = m_entAnswer->font();
    f3.setPointSize(18);
    m_entAnswer->setFont(f3);
    m_entAnswer->setAlignment(Qt::AlignCenter);
    playLayout->addWidget(m_entAnswer);
    
    // Play/Check Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_btnPlay = new QPushButton("Play Challenge");
    m_btnCheck = new QPushButton("Check Answer");
    QPushButton *btnStats = new QPushButton("Statistics");
    
    btnLayout->addWidget(m_btnPlay);
    btnLayout->addWidget(m_btnCheck);
    btnLayout->addWidget(btnStats);
    
    connect(btnStats, &QPushButton::clicked, this, &MainWindow::toggleStatistics);
    playLayout->addLayout(btnLayout);
    
    layout->addWidget(playBox);
}

// Connect Signals to Slots
void MainWindow::setupConnections()
{
    // Connect Button
    connect(m_btnConnect, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    // Serial Line Received -> update UI
    connect(m_serial, &SerialManager::lineReceived, this, &MainWindow::onSerialLineReceived);
    // Serial Text Received -> update RX log immediately
    connect(m_serial, &SerialManager::textReceived, this, &MainWindow::onSerialTextReceived);
    
    // Real-Time Sidetone
    connect(m_serial, &SerialManager::toneStartReceived, this, [this](){
        if (m_chkOffline->isChecked()) {
             int tone = m_spinOfflineTone->value();
             m_sound->startTone(tone);
        }
    });
    connect(m_serial, &SerialManager::toneStopReceived, this, [this](){
         // Stop regardless of check? Or only if checked?
         // Safest to always stop if we started.
         m_sound->stopTone();
    });

    // Serial Connected -> update status label
    connect(m_serial, &SerialManager::connected, this, [this](){
        m_lblStatus->setText("CONNECTED");
        m_lblStatus->setStyleSheet("color: green; font-weight: bold;");
    });
    // Serial Disconnected -> update status label
    connect(m_serial, &SerialManager::disconnected, this, [this](){
        m_lblStatus->setText("OFFLINE");
        m_lblStatus->setStyleSheet("color: red; font-weight: bold;");
    });
    // Offline Checkbox
    connect(m_chkOffline, &QCheckBox::toggled, this, &MainWindow::toggleOfflineUi);
    
    // Spacing Checkbox
    connect(m_chkAdjustableSpacing, &QCheckBox::toggled, this, &MainWindow::toggleSpacingUi);

    // Trainer Actions
    // Play Button -> Generate/Play Drill
    connect(m_btnPlay, &QPushButton::clicked, this, &MainWindow::playDrill);
    // Check Button -> Verify Answer
    connect(m_btnCheck, &QPushButton::clicked, this, &MainWindow::checkAnswer);
    // Enter key in answer box -> Verify Answer
    connect(m_entAnswer, &QLineEdit::returnPressed, this, &MainWindow::checkAnswer);
    
    // Audio Controls
    connect(m_sliderVolume, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
    connect(m_comboAudioDevice, &QComboBox::currentIndexChanged, this, &MainWindow::onAudioDeviceChanged);
}

// Refresh Serial Ports List
void MainWindow::refreshPorts()
{
    m_portCombo->clear();
    m_portCombo->addItems(m_serial->getAvailablePorts());
}

// Connect/Disconnect Serial Port
void MainWindow::toggleConnection()
{
    if (m_serial->isConnected()) {
        // DISCONNECT
        m_serial->disconnectFromPort();
        m_btnConnect->setText("Connect");
        m_lblStatus->setText("OFFLINE");
        m_lblStatus->setStyleSheet("color: red; font-weight: bold;");
    } else {
        // CONNECT
        QString port = m_portCombo->currentText();
        int baud = m_baudCombo->currentText().toInt();
        
        if (m_serial->connectToPort(port, baud)) {
            m_btnConnect->setText("Disconnect");
            m_lblStatus->setText("CONNECTED");
            m_lblStatus->setStyleSheet("color: green; font-weight: bold;");
        } else {
            // Show error if failed
            QMessageBox::critical(this, "Error", "Could not connect to port.");
        }
    }
}

// Handle incoming serial line
void MainWindow::onSerialLineReceived(QString line)
{
    // Parse Status Updates and update dashboard labels
    bool isSystemMsg = false;
    QString textToRemove;
    
    if (line.contains("WPM set to")) {
        QString val = line.section("WPM set to", 1, 1).trimmed();
        m_lblDashWpm->setText("Speed: " + val + " WPM");
        m_tracker->setCurrentWpm(val.toInt());
        isSystemMsg = true;
        textToRemove = "WPM set to " + val;
    }
    else if (line.contains("Tone set to")) {
        QString val = line.section("Tone set to", 1, 1).trimmed();
        m_lblDashTone->setText("Tone: " + val + " Hz");
        isSystemMsg = true;
        textToRemove = "Tone set to " + val;
    }
    else if (line.contains("Mode set to")) {
        QString val = line.section("Mode set to", 1, 1).trimmed();
        m_lblDashMode->setText("Mode: " + val);
        isSystemMsg = true;
        textToRemove = "Mode set to " + val;
    }
    else if (line.contains("Action:")) {
        // e.g. "Action: Buffer Cleared" - just log it as system msg
        isSystemMsg = true;
        textToRemove = line; // Remove the whole line for actions
    }
    else if (line.startsWith("\nEncoded:")) {
        // Spoiler prevention! Hide the answer from the log.
        isSystemMsg = true;
        textToRemove = line;
    }
    else if (line.contains("[Done]")) {
        // Spoiler prevention! Hide the done message.
        isSystemMsg = true;
        textToRemove = line;
    }

    // New Filter Logic: Retroactive Removal
    // Since we display text immediately (including system msgs), 
    // we now check if we should have hidden this message, and if so, delete it from the log.
    if (isSystemMsg && !m_chkShowSys->isChecked() && !textToRemove.isEmpty()) {
        QTextCursor cursor = m_txtRx->textCursor();
        cursor.movePosition(QTextCursor::End);
        
        QTextDocument *doc = m_txtRx->document();
        // Search backwards from the end to find the most recent occurrence
        QTextCursor highlight = doc->find(textToRemove, cursor, QTextDocument::FindBackward);
        
        if (!highlight.isNull()) {
            highlight.removeSelectedText();
            // Optional: Also try to remove a trailing newline if left behind
            // This is a simple cleanup to avoid leaving empty lines
            QTextCursor cleanupCursor = m_txtRx->textCursor();
            cleanupCursor.movePosition(QTextCursor::End);
            // Select the last character
            cleanupCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            }
        }
    }

// Handle raw text for immediate display
void MainWindow::onSerialTextReceived(QString text)
{
    // Append to RX Log
    m_txtRx->moveCursor(QTextCursor::End);
    m_txtRx->insertPlainText(text);
    m_txtRx->ensureCursorVisible();

    // In TX Mode, route paddle input to Answer Box
    if (m_radioTx->isChecked()) {
        QString current = m_entAnswer->text() + text;
        
        // Sanitize "Encoded:" artifact which might arrive in chunks
        // e.g. "Enc" + "oded: A" -> "Encoded: A" -> "A"
        current.replace("Encoded:", "", Qt::CaseInsensitive);
        // Also remove just "Encoded" if it appears as a whole word (though rare to have without colon)
        // Actually, let's keep it simple. If "Encoded:" is the prefix, remove it.
        
        // Update the box
        m_entAnswer->setText(current);
    }

    // Echo to Audio if Offline Mode is on (PC Sidetone)
    if (m_chkOffline->isChecked()) {
         // Filter out system messages (heuristic: they contain lowercase)
         // Decoded Morse is always [A-Z0-9? ]
         bool isSystem = false;
         for (QChar c : text) {
             if (c.isLower()) {
                 isSystem = true;
                 break;
             }
         }
         
         if (!isSystem) {
             int wpm = m_spinOfflineWpm->value();
             int tone = m_spinOfflineTone->value();
             // Play with 0 extra spacing for immediate feedback
             m_sound->playMorse(text, wpm, tone, 0); 
         }
    }
}

// Toggle Offline Controls visibility
void MainWindow::toggleOfflineUi()
{
    m_offlineControlsInfo->setVisible(m_chkOffline->isChecked());
}

// Toggle Drill UI based on RX vs TX mode
void MainWindow::toggleDrillUi()
{
    bool isTx = m_radioTx->isChecked();
    if (isTx) {
        // TX Mode: Visualize target, user must Key it
        m_lblTargetBig->show();
        m_lblInstruction->setText("Read the letters above and Key them");
        m_entAnswer->setPlaceholderText("Use Paddle...");
    } else {
        // RX Mode: Hide target, play audio, user must Type what they hear
        m_lblTargetBig->hide();
        m_lblInstruction->setText("Press Play, Listen, Type Answer");
        m_entAnswer->setPlaceholderText("Type here...");
    }
}

// Toggle Spacing UI visibility
void MainWindow::toggleSpacingUi()
{
    if (m_spacingContainer) {
        m_spacingContainer->setVisible(m_chkAdjustableSpacing->isChecked());
    }
}

// Generate a target string for the drill
QString MainWindow::generateTarget()
{
    if (m_radioModeWords->isChecked()) {
        // Words Mode: Pick a random word from the utils list
        QStringList words = MorseUtils::getTrainingWords();
        int idx = QRandomGenerator::global()->bounded(words.size());
        return words[idx];
    } else {
        // Random Characters Mode
        int len = m_spinGroupSize->value();
        QString allowed = m_lineAllowedChars->text().toUpper();
        if (allowed.isEmpty()) allowed = "PARIS"; // Fallback
        
        QString res = "";
        for (int i=0; i<len; ++i) {
            int idx = QRandomGenerator::global()->bounded(allowed.length());
            res.append(allowed[idx]);
        }
        return res;
    }
}

// Start a drill
void MainWindow::playDrill()
{
    // 1. Generate Target
    m_currentTarget = generateTarget();
    // 2. Clear previous answer/UI
    m_entAnswer->clear();
    m_lblFeedback->setText("Playing...");
    m_lblFeedback->setStyleSheet("color: black; font-weight: bold;");
    
    if (m_radioTx->isChecked()) {
        // TX Mode: Show target immediately, don't play audio
        m_lblTargetBig->setText(m_currentTarget);
        m_lblFeedback->setText("Go ahead! Key it.");
    } else {
        // RX Mode: Hide target, play audio
        m_lblTargetBig->clear();
        
        if (m_chkOffline->isChecked()) {
             // Play using PC Audio
             int wpm = m_spinOfflineWpm->value();
             int tone = m_spinOfflineTone->value();
             
             int extraSpacing = 0;
             if (m_chkAdjustableSpacing->isChecked()) {
                 extraSpacing = m_spinSpacingMs->value();
             }
             
             m_sound->playMorse(m_currentTarget, wpm, tone, extraSpacing);
             m_tracker->setCurrentWpm(wpm);
        } else {
             // Online Mode (External Device)
             if (m_chkAdjustableSpacing->isChecked()) {
                 // Use Client-Side Spacing
                 m_pendingDrillText = m_currentTarget;
                 m_drillCharIdx = 0;
                 sendNextDrillChar();
             } else {
                 // Standard Send (Device handles timing)
                 m_serial->sendCommand(m_currentTarget);
             }
        }
    }
}

void MainWindow::sendNextDrillChar()
{
    if (m_drillCharIdx >= m_pendingDrillText.length()) {
        return; // Done
    }
    
    QChar c = m_pendingDrillText[m_drillCharIdx];
    m_drillCharIdx++;
    
    // Send character
    m_serial->sendCommand(QString(c));
    
    // Calculate Duration
    int wpm = 20; // Default fallback
    // Try to parse WPM from dashboard label if possible, or assume device default.
    // Easier: Use the Offline WPM spinner as a "Reference WPM" for spacing calc?
    // Or assume standard 20? 
    // User sets speed on device usually. But for spacing to be proportional, we need to know it.
    // If we don't know device speed, we can't calculate perfect spacing relative to it.
    // However, user specifically wants "Adjustable Time Spacing".
    // We can just interpret the "Extra Spacing" as pure delay on top of some estimated char duration.
    // Let's use 20 WPM as a safe estimate or valid WPM from offline spinner if reasonable.
    // Better: Use `m_tracker->getCurrentWpm()` if available, else 20.
    int trackedWpm = m_tracker->getCurrentWpm();
    if (trackedWpm > 0) wpm = trackedWpm;
    
    double dotMs = 1200.0 / wpm;
    
    // Calculate char Morse duration
    // Dot = 1 unit, Dash = 3 units, Intrachar gap = 1 unit.
    // But we are sending char by char. Device plays the char.
    // We need to wait for Device to FINISH playing char + standard gap + extra gap.
    QString code = MorseUtils::getMorseMap().value(c.toUpper());
    double units = 0;
    if (c == ' ') {
        units = 7; // Word gap
    } else {
        for (int i=0; i<code.length(); ++i) {
            if (code[i] == '.') units += 1;
            else if (code[i] == '-') units += 3;
            
            if (i < code.length() - 1) units += 1; // Gap between parts
        }
        // Standard gap after char is 3 units
        units += 3; 
    }
    
    int charDuration = (int)(units * dotMs);
    int extra = m_spinSpacingMs->value();
    
    m_drillTimer->start(charDuration + extra);
}

// Check the user's answer
void MainWindow::checkAnswer()
{
    // Normalize user input
    QString ans = m_entAnswer->text();
    // Safety sanitize
    ans.replace("Encoded:", "", Qt::CaseInsensitive);
    ans = ans.trimmed().toUpper();
    
    // Compare Match
    // Live Stats Update
    m_tracker->recordAttempt(m_currentTarget, ans);
    
    // Comparison Logic
    QString finalAns = ans;
    QString finalTarget = m_currentTarget;
    
    // Handle Ignore Spacing
    if (m_chkIgnoreSpacing->isChecked()) {
        finalAns.replace(" ", "");
        finalTarget.replace(" ", "");
    }
    
    if (finalAns == finalTarget) {
        m_lblFeedback->setText("CORRECT! ✅");
        m_lblFeedback->setStyleSheet("color: green; font-weight: bold;");
    } else {
        m_lblFeedback->setText("WRONG ❌ (You: '" + ans + "' -> Wanted: '" + m_currentTarget + "')");
        m_lblFeedback->setStyleSheet("color: red; font-weight: bold;");
    }
    
    // Live Stats Update
    if (m_statsWindow && m_statsWindow->isVisible()) {
        m_statsWindow->refreshData();
    }
    
    // Auto-clear answer (User requested always clear)
    m_entAnswer->clear();
}

// Send manual serial command from Dashboard
void MainWindow::sendSerialCommand()
{
    QString text = m_txtTx->toPlainText();
    if (!text.isEmpty()) {
        m_serial->sendCommand(text);
        m_txtTx->clear();
    }
}

// Clear the Receive Log
void MainWindow::clearRxLog()
{
    m_txtRx->clear();
}

// Show/Hide Cheat Sheet
void MainWindow::toggleCheatSheet()
{
    // Lazy initialization
    if (!m_cheatSheet) {
        m_cheatSheet = new CheatSheetWindow(this);
    }
    
    // Toggle visibility
    if (m_cheatSheet->isVisible()) {
        m_cheatSheet->hide();
    } else {
        m_cheatSheet->show();
        m_cheatSheet->raise();
        m_cheatSheet->activateWindow();
    }
}

// Show/Hide Statistics Window
void MainWindow::toggleStatistics()
{
    // Lazy initialization
    if (!m_statsWindow) {
        m_statsWindow = new StatisticsWindow(m_tracker, this);
    }

    // Toggle visibility
    if (m_statsWindow->isVisible()) {
        m_statsWindow->hide();
    } else {
        m_statsWindow->refreshData();
        m_statsWindow->show();
        m_statsWindow->raise();
    }
}

// Audio Volume Changed
void MainWindow::onVolumeChanged(int value)
{
    qreal vol = value / 100.0;
    m_sound->setVolume(vol);
}

// Audio Device Changed
void MainWindow::onAudioDeviceChanged(int index)
{
    if (index < 0) return;
    QAudioDevice device = m_comboAudioDevice->itemData(index).value<QAudioDevice>();
    m_sound->setAudioDevice(device);
}
