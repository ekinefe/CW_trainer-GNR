#include "SoundGenerator.h"
#include "MorseUtils.h"
#include <qmath.h>
#include <QAudioFormat>
#include <QMediaDevices>
#include <QDebug>

// Constructor for SoundGenerator
// Initializes parent class and sets pointers to nullptr
SoundGenerator::SoundGenerator(QObject *parent) : QObject(parent), m_audioSink(nullptr), m_buffer(nullptr)
{
}

// Destructor for SoundGenerator
// cleans up the audio sink and buffer if they exist
SoundGenerator::~SoundGenerator()
{
    // Check if audio sink exists
    if (m_audioSink) {
        // Stop playback
        m_audioSink->stop();
        // Delete the object to free memory
        delete m_audioSink;
    }
    // Check if buffer exists
    if (m_buffer) {
        // Delete the buffer to free memory
        delete m_buffer;
    }
}

// Play Morse code sound for the given text
// text: The string to convert to Morse code
// wpm: Words Per Minute (determines speed)
// toneHz: The frequency of the beep in Hertz
// extraSpacingMs: Extra silence between chars in milliseconds
void SoundGenerator::playMorse(const QString &text, int wpm, int toneHz, int extraSpacingMs)
{
    // Stop previous playback if any
    if (m_audioSink) {
        m_audioSink->stop();
        delete m_audioSink;
        m_audioSink = nullptr;
    }
    // Clean up previous buffer
    if (m_buffer) {
        m_buffer->close();
        delete m_buffer;
    }

    // Generate the raw audio data for the Morse sequence
    QByteArray data = generateAudioData(text, wpm, toneHz, extraSpacingMs);
    
    // Create a new QBuffer to hold the audio data
    m_buffer = new QBuffer();
    // Set the data into the buffer
    m_buffer->setData(data);
    // Open the buffer in Read-Only mode so the audio sink can read from it
    m_buffer->open(QIODevice::ReadOnly);

    // Setup Audio Format parameters
    QAudioFormat format;
    format.setSampleRate(44100); // Standard CD quality sample rate
    format.setChannelCount(1);   // Mono audio
    format.setSampleFormat(QAudioFormat::Int16); // 16-bit PCM data
    
    // Get the configured audio output device
    QAudioDevice device = m_device;
    if (device.isNull()) device = QMediaDevices::defaultAudioOutput();

    // Check if the device supports the requested format
    if (!device.isFormatSupported(format)) {
        qWarning() << "Default format not supported, trying preferred";
        // Fallback to the device's preferred format
        format = device.preferredFormat();
    }

    // Create a new Audio Sink with the device and format
    m_audioSink = new QAudioSink(device, format, this);
    
    // Set Volume
    m_audioSink->setVolume(m_volume);
    
    // Start playback from the buffer
    m_audioSink->start(m_buffer);
}

void SoundGenerator::setVolume(qreal volume)
{
    m_volume = qBound(0.0, volume, 1.0);
    if (m_audioSink) {
        m_audioSink->setVolume(m_volume);
    }
}

void SoundGenerator::setAudioDevice(const QAudioDevice &device)
{
    m_device = device;
    // Note: Changing device during playback requires restart, 
    // but simplified logic updates it for next play.
    // Ideally we could stop and restart if playing, but playMorse recreates sink anyway.
}

// Start a continuous tone (Real-Time Sidetone)
void SoundGenerator::startTone(int toneHz)
{
    // If already playing, do nothing (or update frequency if needed, but let's keep simple)
    if (m_audioSink && m_audioSink->state() == QAudio::ActiveState) return;

    // Use infinite buffer technique or long buffer. 
    // Simplest for low latency: Open QAudioSink in Push mode (IODevice) or use infinite looper.
    // Actually, for simple sidetone, we can just play a very long tone buffer and stop it when needed.
    // Let's generate a 10-second tone buffer. That should be enough for any dash.
    
    // Stop previous
    if (m_audioSink) {
        m_audioSink->stop();
        delete m_audioSink;
        m_audioSink = nullptr;
    }
    if (m_buffer) {
        m_buffer->close();
        delete m_buffer;
    }
    
    // Generate 5 seconds of tone (e.g., essentially infinite for a dash)
    QByteArray data = createTone(5.0, toneHz, 44100);
    
    m_buffer = new QBuffer();
    m_buffer->setData(data);
    m_buffer->open(QIODevice::ReadOnly);
    
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);
    
    QAudioDevice device = m_device;
    if (device.isNull()) device = QMediaDevices::defaultAudioOutput();
    if (!device.isFormatSupported(format)) format = device.preferredFormat();
    
    m_audioSink = new QAudioSink(device, format, this);
    m_audioSink->setVolume(m_volume);
    m_audioSink->start(m_buffer);
}

void SoundGenerator::stopTone()
{
    if (m_audioSink) {
        m_audioSink->stop();
        // Don't delete immediately to avoid overhead/clicks if restarting soon? 
        // Actually, stopping is fine.
    }
}

// Generate the complete byte array of audio for the text
QByteArray SoundGenerator::generateAudioData(const QString &text, int wpm, int toneHz, int extraSpacingMs)
{
    int sampleRate = 44100; // Fixed sample rate
    // Calculate dot duration in seconds based on WPM (Paris standard: 50 dots = 1 word)
    // Formula: 60 seconds / (50 * WPM) = 1.2 / WPM
    double dotLen = 1.2 / double(wpm);
    
    QByteArray audio; // Buffer to accumulate audio data
    
    // Pre-generate sound chunks for efficiency
    QByteArray dotSound = createTone(dotLen, toneHz, sampleRate);
    QByteArray dashSound = createTone(dotLen * 3, toneHz, sampleRate); // Dash is 3 dots
    QByteArray elemGap = createSilence(dotLen, sampleRate); // Gap between parts of a char is 1 dot
    QByteArray charGap = createSilence(dotLen * 3, sampleRate); // Gap between chars is 3 dots (handled by logic)
    QByteArray wordGap = createSilence(dotLen * 7, sampleRate); // Gap between words is 7 dots
    
    // Extra custom spacing
    QByteArray customSpacing;
    if (extraSpacingMs > 0) {
        customSpacing = createSilence(extraSpacingMs / 1000.0, sampleRate);
    }

    // Get the character map from MorseUtils
    auto morseMap = MorseUtils::getMorseMap();

    // Loop through each character in the input text
    for (int i = 0; i < text.length(); ++i) {
        QChar c = text[i].toUpper(); // Convert to uppercase
        
        // Handle space (word separator)
        if (c == ' ') {
            audio.append(wordGap);
            // Apply extra spacing to words too if desired, or just leave as word gap
            // Usually Farnsworth adds to characters, effectively slowing words too.
            if (extraSpacingMs > 0) audio.append(customSpacing); 
            continue;
        }

        // Look up Morse code for character
        QString code = morseMap.value(c);
        // Skip unknown characters
        if (code.isEmpty()) continue;

        // Loop through dots and dashes in the code
        for (int j = 0; j < code.length(); ++j) {
            if (code[j] == '.') audio.append(dotSound);
            else if (code[j] == '-') audio.append(dashSound);
            
            // Add gap after every symbol (dot or dash)
            // Note: This adds a gap even after the last symbol of a character
            audio.append(elemGap);
        }
        
        // Add inter-character gap
        // We already added 1 'elemGap' after the last symbol.
        // Standard spacing between characters is 3 dots.
        // So we need to add 2 more dots of silence.
        audio.append(createSilence(dotLen * 2, sampleRate)); 
        
        // Add custom extra spacing
        if (extraSpacingMs > 0) {
            audio.append(customSpacing);
        }
    }
    
    // Return the total generated audio data
    return audio;
}

// Create a sine wave tone
QByteArray SoundGenerator::createTone(double durationS, int toneHz, int sampleRate)
{
    // Calculate total number of samples
    int numSamples = int(sampleRate * durationS);
    QByteArray chunk;
    chunk.resize(numSamples * 2); // Resize buffer (2 bytes per sample for 16-bit)
    
    // Pointer to treat data as signed short (16-bit integer)
    signed short *ptr = reinterpret_cast<signed short*>(chunk.data());
    
    // Generate samples
    for (int i = 0; i < numSamples; ++i) {
        // Current time 't'
        double t = double(i) / sampleRate;
        // Generate sine wave value, scale to 16-bit range
        ptr[i] = static_cast<signed short>(32767.0 * qSin(2.0 * M_PI * toneHz * t));
    }
    return chunk;
}

// Create silence (zeros)
QByteArray SoundGenerator::createSilence(double durationS, int sampleRate)
{
    // Calculate total number of samples
    int numSamples = int(sampleRate * durationS);
    // Create a byte array of zeros
    QByteArray chunk(numSamples * 2, 0); // 2 bytes per sample, initialized to 0
    return chunk;
}
