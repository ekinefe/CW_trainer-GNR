#include "CheatSheetWindow.h"
#include "MorseUtils.h"

// Constructor implementation
CheatSheetWindow::CheatSheetWindow(QWidget *parent) : QDialog(parent)
{
    // Set the window title
    setWindowTitle("Morse Code Cheat Sheet");
    // Set default window size
    resize(600, 400);
    // Initialize the UI elements
    setupUi();
}

// Method to setup the grid layout of Morse symbols
void CheatSheetWindow::setupUi()
{
    // Create a grid layout for this dialog
    QGridLayout *layout = new QGridLayout(this);
    
    // Retrieve the Morse map from utils
    auto map = MorseUtils::getMorseMap();
    // Get the list of characters (keys)
    QList<QChar> keys = map.keys();
    // Sort keys alphabetically
    std::sort(keys.begin(), keys.end());
    
    // Variables for grid positioning
    int row = 0;
    int col = 0;
    int maxRows = 17; // Number of items per column
    
    // Iterate through each character in the sorted list
    for (QChar key : keys) {
        // Create a display string: "Character   Code"
        QString labelText = QString("%1   %2").arg(key).arg(map[key]);
        
        // Create a new label with the text
        QLabel *lbl = new QLabel(labelText);
        
        // Configure font for better readability
        QFont f = lbl->font();
        // f.setPointSize(12);
        f.setPointSize(15);
        f.setBold(true);
        f.setFamily("Consolas"); // Monospace font for alignment
        lbl->setFont(f);
        
        // Add label to grid at current position
        layout->addWidget(lbl, row, col);
        
        // Calculate next position
        row++;
        // Wrap to next column if row limit reached
        if (row >= maxRows) {
            row = 0;
            col++;
        }
    }
}
