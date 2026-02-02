#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H

// Include necessary Qt multimedia and core classes
#include <QObject>
#include <QAudioSink>
#include <QBuffer>
#include <QByteArray>
#include <QMediaDevices>
#include <QThread>

// The SoundGenerator class is responsible for generating and playing Morse code audio
class SoundGenerator : public QObject
{
    // Q_OBJECT macro is required for signals and slots
    Q_OBJECT
public:
    // Constructor: Initializes the sound generator with an optional parent
    explicit SoundGenerator(QObject *parent = nullptr);
    
    // Destructor: Cleans up resources
    ~SoundGenerator();

    // Method to play a Morse code string with specified WPM (Words Per Minute) and frequency
    // extraSpacingMs: Additional silence between characters (Farnsworth spacing)
    // Method to play a Morse code string with specified WPM (Words Per Minute) and frequency
    // extraSpacingMs: Additional silence between characters (Farnsworth spacing)
    void playMorse(const QString &text, int wpm, int toneHz, int extraSpacingMs = 0);

    // Real-Time Tone Control
    void startTone(int toneHz);
    void stopTone();

    // Set the volume (0.0 to 1.0)
    void setVolume(qreal volume);

    // Set the audio output device
    void setAudioDevice(const QAudioDevice &device);

private:
    // Helper method to generate the raw audio data for the entire text
    QByteArray generateAudioData(const QString &text, int wpm, int toneHz, int extraSpacingMs);
    
    // Helper method to create a sine wave tone for a specific duration and frequency
    QByteArray createTone(double durationS, int toneHz, int sampleRate);
    
    // Helper method to create silence (for spacing) for a specific duration
    QByteArray createSilence(double durationS, int sampleRate);
    
    // Pointer to the audio sink (output device interface)
    QAudioSink *m_audioSink;
    
    // Buffer to hold audio data during playback
    QBuffer *m_buffer;
    
    // Volume level (0.0 to 1.0)
    qreal m_volume = 1.0;
    
    // Selected Audio Device
    QAudioDevice m_device = QMediaDevices::defaultAudioOutput();
};

#endif // SOUNDGENERATOR_H
