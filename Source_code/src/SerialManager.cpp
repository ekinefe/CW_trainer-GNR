#include "SerialManager.h"
#include <QDebug>

// Constructor
SerialManager::SerialManager(QObject *parent) : QObject(parent)
{
    // Create new QSerialPort instance
    m_serial = new QSerialPort(this);
    // Connect readyRead signal to onReadyRead slot to handle incoming data
    connect(m_serial, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
    // Connect errorOccurred signal to onError slot to handle errors
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialManager::onError);
}

// Destructor
SerialManager::~SerialManager()
{
    // Ensure we disconnect gracefully before destruction
    disconnectFromPort();
}

// Get list of available serial ports
QStringList SerialManager::getAvailablePorts()
{
    QStringList ports;
    // Get info on available ports from system
    const auto infos = QSerialPortInfo::availablePorts();
    // Iterate through ports and add names to list
    for (const QSerialPortInfo &info : infos) {
        ports.append(info.portName());
    }
    return ports;
}

// Connect to a specific port
bool SerialManager::connectToPort(const QString &portName, int baudRate)
{
    // Close existing connection if any
    if (m_serial->isOpen()) {
        m_serial->close();
    }

    // Configure port settings
    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(QSerialPort::Data8); // 8 Data bits
    m_serial->setParity(QSerialPort::NoParity); // No Parity
    m_serial->setStopBits(QSerialPort::OneStop); // 1 Stop bit
    m_serial->setFlowControl(QSerialPort::NoFlowControl); // No Flow Control

    // Attempt to open port in Read/Write mode
    if (m_serial->open(QIODevice::ReadWrite)) {
        emit connected();
        return true;
    } else {
        // Emit error if connection fails
        emit errorOccurred(m_serial->errorString());
        return false;
    }
}

// Disconnect from current port
void SerialManager::disconnectFromPort()
{
    if (m_serial->isOpen()) {
        m_serial->close();
        emit disconnected();
    }
}

// Check connection status
bool SerialManager::isConnected() const
{
    return m_serial->isOpen();
}

// Send command string to device
void SerialManager::sendCommand(const QString &command)
{
    if (m_serial->isOpen()) {
        QByteArray data = command.toUtf8();
        // Append newline if missing (protocol requirement)
        if (!data.endsWith('\n')) {
            data.append('\n');
        }
        // Write data to serial port
        m_serial->write(data);
    }
}

// Handle incoming data
void SerialManager::onReadyRead()
{
    // Read all available data immediately (don't wait for newlines)
    QByteArray data = m_serial->readAll();
    if (!data.isEmpty()) {
        // Convert raw bytes to string
        QString str = QString::fromUtf8(data);
        
        // Scan for Real-Time Tone Tokens
        if (str.contains('[')) {
            emit toneStartReceived();
            str.replace("[", ""); // Remove token
        }
        if (str.contains(']')) {
            emit toneStopReceived();
            str.replace("]", ""); // Remove token
        }

        // If string became empty, return
        if (str.isEmpty()) return;

        // Emit raw text immediately for UI updates
        emit textReceived(str);
        
        // Split by newlines for line-by-line emission
        QStringList lines = str.split('\n', Qt::KeepEmptyParts);
        
        for (int i = 0; i < lines.size(); ++i) {
            QString line = lines[i];
            
            // If this is the last chunk and it didn't end with a newline, it's incomplete
            if (i == lines.size() - 1 && !str.endsWith('\n')) {
                // Buffer it for next read
                m_buffer += line;
            } else {
                // Complete line - combine with buffer if any
                QString completeLine = m_buffer + line;
                m_buffer.clear(); // Clear buffer
                completeLine = completeLine.trimmed(); // Remove whitespace
                
                // Emit signal if line is not empty
                if (!completeLine.isEmpty()) {
                    emit lineReceived(completeLine);
                }
            }
        }
    }
}

// Handle serial errors
void SerialManager::onError(QSerialPort::SerialPortError error)
{
    // Ignore NoError
    if (error == QSerialPort::NoError) return;
    
    // Don't emit error on expected close (e.g. user initiated)
    // ResourceError usually happens if device is unplugged
    if (error == QSerialPort::ResourceError || error == QSerialPort::PermissionError) {
         disconnectFromPort();
         emit errorOccurred(m_serial->errorString());
    }
}
