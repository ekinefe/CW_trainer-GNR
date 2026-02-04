#ifndef STATISTICSTRACKER_H
#define STATISTICSTRACKER_H

// Include standard Qt classes for string, map, and datetime handling
#include <QString>
#include <QMap>
#include <QDateTime>

// Structure to track statistics for an individual character
struct CharStats {
    int given = 0;   // Number of times the character was presented
    int correct = 0; // Number of times it was typed correctly
    int wrong = 0;   // Number of times it was typed incorrectly
};

// Structure to track statistics for a complete item (word or group)
struct ItemStats {
    int given = 0;   // Number of times this specific item was presented
    int correct = 0; // Number of times it was completely correct
    int wrong = 0;   // Number of times it had errors
};

// Class responsible for tracking user performance statistics
class StatisticsTracker
{
public:
    // Constructor: Initializes the tracker
    StatisticsTracker();
    
    // Records a training attempt comparing the target string vs user input
    // Returns a pair containing <number_of_matched_chars, total_target_chars>
    QPair<int, int> recordAttempt(const QString &target, const QString &user);
    
    // Saves the current session statistics to a CSV file
    void saveSession(int wpm, int tone, const QString &mode);

    // Returns the list of characters tracked for statistics
    static QList<QChar> getTrackedChars();

    // Accessors for Live Data
    QMap<QChar, CharStats> getCharStats() const;
    int getTotalAttempts() const;
    int getCorrectCount() const;
    QDateTime getStartTime() const;

    // Set/Get the current WPM for live tracking
    void setCurrentWpm(int wpm);
    int getCurrentWpm() const;

private:
    // Timestamp when the session started
    QDateTime m_startTime;
    // Counter for total attempts made in this session
    int m_totalAttempts;
    // Counter for total correct attempts (perfect matches)
    int m_correctCount;
    // Counter for total incorrect attempts
    int m_wrongCount;
    // Current WPM setting for the session
    int m_currentWpm;
    
    // Map storing stats per character (e.g., 'A', 'B', '1')
    QMap<QChar, CharStats> m_charStats;
    // Map storing stats per specific item/word
    QMap<QString, ItemStats> m_itemStats;
    
    // Helper method to calculate Diff/LCS (Longest Common Subsequence)
    // Used to intelligently attribute correct/wrong stats to individual characters 
    // even when there are insertions/deletions/misalignments
    void updateCharStatsSmart(const QString &target, const QString &user);
};

#endif // STATISTICSTRACKER_H
