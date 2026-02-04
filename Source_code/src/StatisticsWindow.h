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
