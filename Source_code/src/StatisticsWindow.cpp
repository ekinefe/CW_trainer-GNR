#include "StatisticsWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QMap>
#include <algorithm>
#include <QDebug>
#include <QDate>
#include "StatisticsTracker.h" // for getTrackedChars

// --- TrendChart Implementation ---

TrendChart::TrendChart(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(250);
}

void TrendChart::setData(const QList<SessionData> &data)
{
    m_data = data;
    update(); // Trigger repaint
}

void TrendChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // Margins
    int padL = 40;
    int padR = 40;
    int padT = 20;
    int padB = 30;

    int plotW = w - padL - padR;
    int plotH = h - padT - padB;

    // Draw Background
    painter.fillRect(rect(), Qt::white);

    // Draw Axes
    painter.setPen(QPen(QColor("#333"), 2));
    painter.drawLine(padL, h - padB, w - padR, h - padB); // X Axis
    painter.drawLine(padL, padT, padL, h - padB);         // Y Axis Left (Acc)
    painter.drawLine(w - padR, padT, w - padR, h - padB); // Y Axis Right (WPM)

    // Draw Gridlines and Labels
    painter.setFont(QFont("Arial", 8));
    for (int i = 0; i < 5; ++i) {
        int y = (h - padB) - (i * plotH / 4);
        
        // Gridline
        QPen gridPen(QColor("#eee"));
        gridPen.setStyle(Qt::DashLine);
        painter.setPen(gridPen);
        painter.drawLine(padL, y, w - padR, y);

        // Acc Label (0, 25, 50, 75, 100)
        painter.setPen(Qt::black);
        painter.drawText(QRect(0, y - 10, padL - 5, 20), Qt::AlignRight | Qt::AlignVCenter, QString::number(i * 25));
    }

    if (m_data.isEmpty()) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, "No Data Available");
        return;
    }

    // Determine Axis Scales
    int maxWpm = 50; // Default min max
    for (const auto &d : m_data) {
        if (d.wpm > maxWpm) maxWpm = d.wpm;
    }
    // Round up to nearest 10
    maxWpm = ((maxWpm + 9) / 10) * 10;

    // Draw WPM Labels
    for (int i = 0; i < 5; ++i) {
        int y = (h - padB) - (i * plotH / 4);
        int wpmVal = i * maxWpm / 4;
        painter.drawText(QRect(w - padR + 5, y - 10, padR, 20), Qt::AlignLeft | Qt::AlignVCenter, QString::number(wpmVal));
    }

    // Plot Data
    int numPts = m_data.size();
    if (numPts < 2) {
         // If only one point, draw it in the middle
         // ... (simplified for now)
    }
    
    double xStep = (numPts > 1) ? (double)plotW / (numPts - 1) : plotW / 2;

    QPolygonF accPoints;
    QPolygonF wpmPoints;

    for (int i = 0; i < numPts; ++i) {
        double x = padL + (i * xStep);
        
        // Accumulate points
        // Acc (0-100)
        double yAcc = (h - padB) - (m_data[i].accuracy / 100.0 * plotH);
        accPoints << QPointF(x, yAcc);

        // WPM (0-maxWpm)
        double yWpm = (h - padB) - ((double)m_data[i].wpm / maxWpm * plotH);
        wpmPoints << QPointF(x, yWpm);
        
        // Draw Dots
        painter.setBrush(QColor("#28a745")); // Green
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(x, yAcc), 3, 3);
        
        painter.setBrush(QColor("#0055aa")); // Blue
        painter.drawEllipse(QPointF(x, yWpm), 3, 3);
    }
    
    // Draw Lines
    painter.setPen(QPen(QColor("#28a745"), 2));
    painter.setBrush(Qt::NoBrush);
    if (accPoints.size() > 1) painter.drawPolyline(accPoints);
    
    painter.setPen(QPen(QColor("#0055aa"), 2));
    if (wpmPoints.size() > 1) painter.drawPolyline(wpmPoints);

    // Legend
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    painter.setPen(QColor("#28a745"));
    painter.drawText(padL + 10, padT, "Accuracy %");
    
    painter.setPen(QColor("#0055aa"));
    painter.drawText(padL + 90, padT, "WPM");
}

// --- ProblemCharChart Implementation ---

ProblemCharChart::ProblemCharChart(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(200);
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

    int w = width();
    int h = height();

    // Margins
    int padL = 40;
    int padR = 20;
    int padT = 30;
    int padB = 10;

    int plotW = w - padL - padR;
    int plotH = h - padT - padB;

    painter.fillRect(rect(), Qt::white);
    
    // Title
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, "Most Problematic Characters (Error %)");

    if (m_data.isEmpty()) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, "No Errors Found");
        return;
    }

    int numBars = m_data.size();
    double barH = (double)plotH / numBars;
    double barGap = barH * 0.2;
    double actualBarH = barH - barGap;

    for (int i = 0; i < numBars; ++i) {
        double yTop = padT + (i * barH) + (barGap / 2);
        
        // Char Label
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(QRect(0, yTop, padL - 10, actualBarH), Qt::AlignRight | Qt::AlignVCenter, QString(m_data[i].character));

        // Bar (Red)
        double barLen = (m_data[i].errorRate / 100.0) * plotW;
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

