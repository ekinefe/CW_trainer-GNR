#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

// Include QObject for signals/slots functionality
#include <QObject>
// Include QSerialPort for serial communication operations
#include <QSerialPort>
// Include QSerialPortInfo to list available ports
#include <QSerialPortInfo>
// Include QStringList for handling lists of strings
#include <QStringList>

// Class responsible for managing serial port connections and data transfer
class SerialManager : public QObject
{
    Q_OBJECT // Macro required for Qt signals and slots
public:
    // Constructor: Initializes the serial manager
    explicit SerialManager(QObject *parent = nullptr);
    // Destructor: Cleans up serial resources
    ~SerialManager();

    // Returns a list of names of available serial ports
    QStringList getAvailablePorts();
    
    // Attempts to connect to a specific port with a given baud rate
    // Returns true if successful, false otherwise
    bool connectToPort(const QString &portName, int baudRate);
    
    // Disconnects from the current port if connected
    void disconnectFromPort();
    
    // Checks if a serial connection is currently active
    bool isConnected() const;
    
    // Sends a text command to the connected serial device
    void sendCommand(const QString &command);

signals:
    // Emitted when raw text is received (for immediate display)
    // Emitted when raw text is received (for immediate display)
    void textReceived(QString text);
    // Emitted when a complete line of text is received (terminated by newline)
    void lineReceived(QString line);
    
    // Real-Time Tone Signals
    void toneStartReceived();
    void toneStopReceived();
    
    // Emitted when connection is successfully established
    // Emitted when connection is successfully established
    void connected();
    // Emitted when disconnected from the port
    void disconnected();
    // Emitted when an error occurs, providing an error message
    void errorOccurred(QString msg);

private slots:
    // Slot called when data is ready to be read from the serial port
    void onReadyRead();
    // Slot called when a serial port error occurs
    void onError(QSerialPort::SerialPortError error);

private:
    // Pointer to the QSerialPort instance
    QSerialPort *m_serial;
    // Buffer to store incoming data until a newline is received
    QString m_buffer; 
};

#endif // SERIALMANAGER_H
