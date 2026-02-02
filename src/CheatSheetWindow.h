#ifndef CHEATSHEETWINDOW_H
#define CHEATSHEETWINDOW_H

// Include standard QDialog class and UI elements
#include <QDialog>
#include <QGridLayout>
#include <QLabel>

// A dialog window that displays a reference list of Morse code characters
class CheatSheetWindow : public QDialog
{
    Q_OBJECT // Required for Qt macros
public:
    // Constructor: Takes an optional parent widget
    explicit CheatSheetWindow(QWidget *parent = nullptr);

private:
    // Internal method to build the user interface programmatically
    void setupUi();
};

#endif // CHEATSHEETWINDOW_H
