#include "StatisticsTracker.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <algorithm>
#include <vector>

// Constructor for StatisticsTracker
// Initializes the start time to now and counters to zero
StatisticsTracker::StatisticsTracker()
    : m_startTime(QDateTime::currentDateTime()), 
      m_totalAttempts(0), m_correctCount(0), m_wrongCount(0), m_currentWpm(20)
{
}

// Record a user's attempt at typing the target string
// target: The string the user was supposed to type
// user: The string the user actually typed
// Returns pair: <number of correct characters, length of target string>
QPair<int, int> StatisticsTracker::recordAttempt(const QString &target, const QString &user)
{
    // Normalize inputs: trim whitespace and convert to uppercase
    QString t = target.trimmed().toUpper();
    QString u = user.trimmed().toUpper();
    
    // Check for exact match
    bool isCorrect = (t == u);
    
    // Update overall session counters
    m_totalAttempts++;
    if (isCorrect) m_correctCount++; else m_wrongCount++;
    
    // Update statistics for this specific word/item
    m_itemStats[t].given++;
    if (isCorrect) m_itemStats[t].correct++; else m_itemStats[t].wrong++;
    
    // Update smart character-level statistics (handling alignment)
    updateCharStatsSmart(t, u);
    
    // Calculate simple match count for immediate feedback using LCS (Longest Common Subsequence)
    // This gives a score of how many characters were "correct" relative to the target sequence
    
    int n = t.length();
    int m = u.length();
    // Create DP table initialized to 0. Size (n+1) x (m+1)
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

    // Fill DP table
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            // If characters match
            if (t[i-1] == u[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                // Take max of previous states (skip char in target OR skip char in user)
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }
    // Result is in the bottom-right cell
    int matched = dp[n][m];
    
    // Return the number of matched characters and the total expected length
    return qMakePair(matched, n);
}

// Smartly update character stats using LCS Backtracking
// This allows us to know WHICH characters were typed correctly vs missed/wrong
void StatisticsTracker::updateCharStatsSmart(const QString &target, const QString &user)
{
    int n = target.length();
    int m = user.length();
    // Create DP table for LCS
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

    // Standard LCS algorithm
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (target[i-1] == user[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }
    
    // Backtrack from the end to find which specific characters in Target were matched
    std::vector<bool> targetMatched(n, false);
    int i = n, j = m;
    while (i > 0 && j > 0) {
        if (target[i-1] == user[j-1]) {
            // Found a match (part of the LCS)
            targetMatched[i-1] = true;
            i--; j--;
        } else if (dp[i-1][j] > dp[i][j-1]) {
            // Moves up: Character in target was not matched (deletion/substitution)
            i--;
        } else {
            // Moves left: Character in user was extra (insertion)
            j--;
        }
    }
    
    // Iterate through original target string to update stats
    for (int k = 0; k < n; ++k) {
        QChar c = target[k];
        // Skip spaces for stats tracking
        if (c == ' ') continue;
        
        // This character was "given" to the user
        m_charStats[c].given++;
        
        if (targetMatched[k]) {
            // It was part of the LCS, specifically matched
            m_charStats[c].correct++;
        } else {
            // It was not matched
            m_charStats[c].wrong++;
        }
    }
}

// Save session statistics to a CSV file
void StatisticsTracker::saveSession(int wpm, int tone, const QString &mode)
{
    // Don't save session if there were no attempts
    if (m_totalAttempts == 0) {
        return;
    }

    // List of characters we track stats for
    QList<QChar> trackedChars = getTrackedChars();

    // Check if we need to upgrade the file format (add missing columns)
    QFile file("statistics.csv");
    bool exists = file.exists();
    bool needsUpgrade = false;

    if (exists) {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString header = in.readLine();
            // Simple check: does the header contain detailed char stats?
            if (!header.contains("A_Total")) {
                needsUpgrade = true;
            }
            // Bug Fix: Check for malformed comma header from previous version
            if (header.contains(",,_Total")) {
               // We need to repair the file
               file.close();
               
               // Read all content
               if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                   QString content = file.readAll();
                   file.close();
                   
                   // Replace bad patterns
                   content.replace(",,_Total", ",COMMA_Total");
                   content.replace(",,_OK", ",COMMA_OK");
                   content.replace(",,_Err", ",COMMA_Err");
                   
                   // Write back
                   if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                       QTextStream outRewrite(&file);
                       outRewrite << content;
                       file.close();
                       
                       // File is now fixed, proceed as normal exists=true
                   }
               }
            } else {
               file.close();
            }
        }
    }

    if (needsUpgrade) {
        // Renaming old file
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        QString backupName = QString("statistics_backup_%1.csv").arg(timestamp);
        QFile::rename("statistics.csv", backupName);
        exists = false; // Treat as new file
    }

    // Open the statistics file in Append mode
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);

        // Write header if file is new
        if (!exists) {
            out << "Date,Time,Duration,Attempts,Correct,Wrong,Accuracy,WPM,Tone,Mode,Item_Stats";
            // Append headers for all tracked characters
            for (QChar c : trackedChars) {
                QString label = c;
                if (c == ',') label = "COMMA";
                out << "," << label << "_Total," << label << "_OK," << label << "_Err";
            }
            out << "\n";
        }

        // Calculate overall accuracy percentage
        double acc = 0;
        if (m_totalAttempts > 0) acc = (double)m_correctCount / m_totalAttempts * 100.0;

        // Calculate duration of session
        qint64 duration = m_startTime.secsTo(QDateTime::currentDateTime());

        // Serialize item stats to a string
        QString itemStr;
        for (auto it = m_itemStats.begin(); it != m_itemStats.end(); ++it) {
            itemStr += QString("%1(G:%2/OK:%3/ERR:%4);").arg(it.key()).arg(it.value().given).arg(it.value().correct).arg(it.value().wrong);
        }

        // Write the CSV row - Standard Stats
        out << m_startTime.toString("yyyy-MM-dd") << ","
            << m_startTime.toString("HH:mm:ss") << ","
            << duration << "," // Duration in seconds
            << m_totalAttempts << ","
            << m_correctCount << ","
            << m_wrongCount << ","
            << QString::number(acc, 'f', 1) << "," // Accuracy with 1 decimal place
            << wpm << ","
            << tone << ","
            << mode << ","
            << itemStr; // Detailed stats

        // Write the CSV row - detailed character stats
        for (QChar c : trackedChars) {
            int total = m_charStats[c].given;
            int ok = m_charStats[c].correct;
            int err = m_charStats[c].wrong;
            out << "," << total << "," << ok << "," << err;
        }

        out << "\n";

        file.close();
    }
}

QList<QChar> StatisticsTracker::getTrackedChars()
{
    // Return A-Z, 0-9, and specific punctuation
    QList<QChar> list;
    // A-Z
    for (char c = 'A'; c <= 'Z'; ++c) list.append(QChar(c));
    // 0-9
    for (char c = '0'; c <= '9'; ++c) list.append(QChar(c));
    // Punctuation
    list.append('?');
    list.append('.');
    list.append(',');
    list.append('/');
    list.append('='); // BT
    list.append('+'); // AR (mapped often to +)

    return list;
}

// --- Accessors for Live Data ---

QMap<QChar, CharStats> StatisticsTracker::getCharStats() const
{
    return m_charStats;
}

int StatisticsTracker::getTotalAttempts() const
{
    return m_totalAttempts;
}

int StatisticsTracker::getCorrectCount() const
{
    return m_correctCount;
}

QDateTime StatisticsTracker::getStartTime() const
{
    return m_startTime;
}

void StatisticsTracker::setCurrentWpm(int wpm)
{
    m_currentWpm = wpm;
}

int StatisticsTracker::getCurrentWpm() const
{
    return m_currentWpm;
}
