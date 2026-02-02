#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Include standard Qt UI and Core headers
#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QCloseEvent>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>

// Include Project Component Headers
#include "SerialManager.h"
#include "SoundGenerator.h"
#include "CheatSheetWindow.h"
#include "StatisticsWindow.h"
#include "StatisticsTracker.h"

// The MainWindow class is the central controller of the application.
// It manages the UI, connects different components (Serial, Audio, Stats),
// and handles user interactions.
class MainWindow : public QMainWindow
{
    Q_OBJECT // Required for Signals and Slots
    
public:
    // Constructor: Sets up the UI and initial state
    MainWindow(QWidget *parent = nullptr);
    // Destructor: Cleans up resources
    ~MainWindow();

private slots:
    // --- Connectivity Slots ---
    // Refreshes the list of available serial ports
    void refreshPorts();
    // Toggles the serial connection (Connect/Disconnect)
    void toggleConnection();
    // Handles data received from the SerialManager
    void onSerialLineReceived(QString line);
    // Handles raw text received from SerialManager (for immediate display)
    void onSerialTextReceived(QString text);
    
    // --- Dashboard Slots ---
    // Sends the text from the TX text box to the serial port
    void sendSerialCommand();
    // Clears the Receive Log text area
    void clearRxLog();
    
    // --- Tools Slots ---
    // Toggles visibility of the Cheat Sheet window
    void toggleCheatSheet();
    // Toggles visibility of the Statistics window
    void toggleStatistics();
    
    // --- Trainer Settings Slots ---
    // Toggles UI elements based on 'Offline' checkbox state
    void toggleOfflineUi();
    // Toggles UI elements based on Drill Type (RX vs TX)
    void toggleDrillUi();
    // Updated Trainer Actions
    // Updated Trainer Actions
    void toggleSpacingUi(); // Toggle visibility of spacing spinner
    
    // Starts a new drill/challenge (Generates target and plays/shows it)
    void playDrill();
    // Checks the user's answer against the current target
    void checkAnswer();
    // Sends the next character in the pending drill (for Online spacing)
    void sendNextDrillChar();

    // Audio Slots
    void onVolumeChanged(int value);
    void onAudioDeviceChanged(int index);

protected:
    // Event handler for window close event (used to save stats)
    void closeEvent(QCloseEvent *event) override;

private:
    // Internal helper to setup the User Interface widgets
    void setupUi();
    // Internal helper to connect signals and slots
    void setupConnections();
    // Internal helper to setup the Dashboard tab
    void setupDashboard(QWidget *parent);
    // Internal helper to setup the Trainer tab
    void setupTrainer(QWidget *parent);
    
    // Helper to generate a random target string based on current settings
    QString generateTarget();

    // --- UI Elements Pointers ---
    QTabWidget *m_tabs; // Main Tab Widget
    
    // Connection Bar Widgets
    QComboBox *m_portCombo; // Dropdown for Port Selection
    QComboBox *m_baudCombo; // Dropdown for Baud Rate
    QPushButton *m_btnConnect; // Connect/Disconnect Button
    QLabel *m_lblStatus; // Status Label (Connected/Offline)
    
    // Dashboard Tab Widgets
    QLabel *m_lblDashWpm; // speed indicator
    QLabel *m_lblDashTone; // tone indicator
    QLabel *m_lblDashMode; // mode indicator
    QCheckBox *m_chkShowSys; // check to show system messages
    QTextEdit *m_txtRx; // Received Text Display (ReadOnly)
    QTextEdit *m_txtTx; // Transmit Text Input
    
    // Trainer Tab - Configuration Widgets
    QRadioButton *m_radioModeWords; // Option for 'Meaningful Words'
    QRadioButton *m_radioModeRandom; // Option for 'Random Characters'
    QSpinBox *m_spinGroupSize; // Spinner for Random Group Length
    QSpinBox *m_spinSetSize; // Spinner for Set Size (number of groups) - *Unused in current logic*
    QRadioButton *m_radioRx; // Option for RX Drill (Listen & Type)
    QRadioButton *m_radioTx; // Option for TX Drill (Read & Key)
    QLineEdit *m_lineAllowedChars; // Input for allowed character set
    QCheckBox *m_chkIgnoreSpacing; // Checkbox to be lenient on spacing - *Unused logic placeholder*
    
    // Adjustable Spacing
    QCheckBox *m_chkAdjustableSpacing; // Checkbox for extra time spacing
    QSpinBox *m_spinSpacingMs; // Spinner for extra spacing in ms
    QWidget *m_spacingContainer; // Container for hiding/showing spacing controls

    QCheckBox *m_chkOffline; // Checkbox to use PC Sound instead of External Device
    QSpinBox *m_spinOfflineWpm; // Spinner for PC Sound WPM
    QSpinBox *m_spinOfflineTone; // Spinner for PC Sound Tone Hz
    QWidget *m_offlineControlsInfo; // Container for WPM/Tone controls (to hide/show)
    
    // Audio Settings
    QSlider *m_sliderVolume;
    QComboBox *m_comboAudioDevice;
    
    // Trainer Tab - Play Area Widgets
    QLabel *m_lblInstruction; // Instruction Text
    QLabel *m_lblTargetBig; // Large Text Label for TX Mode Target
    QLabel *m_lblFeedback; // Feedback Label (Correct/Wrong)
    QLineEdit *m_entAnswer; // Input field for user answer
    QPushButton *m_btnPlay; // Button to start drill
    QPushButton *m_btnCheck; // Button to verify answer
    
    // --- Logic Components ---
    SerialManager *m_serial; // Handles Serial Comm
    SoundGenerator *m_sound; // Handles Audio Generation
    CheatSheetWindow *m_cheatSheet = nullptr; // Pointer to Cheat Sheet Window
    StatisticsWindow *m_statsWindow = nullptr; // Pointer to Stats Window
    StatisticsTracker *m_tracker; // Handles Stats Logic
    
    QString m_currentTarget; // Stores the current drill target string

    // For Device Spacing Control
    QTimer *m_drillTimer;
    QString m_pendingDrillText;
    int m_drillCharIdx;
};

#endif // MAINWINDOW_H
