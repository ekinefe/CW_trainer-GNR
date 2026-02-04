#include "MainWindow.h"
#include <QApplication>

// Main Application Entry Point
int main(int argc, char *argv[])
{
    // Create the Qt Application instance
    QApplication a(argc, argv);
    
    // Create the Main Window of the application
    MainWindow w;
    // Display the main window
    w.show();
    
    // executing the application event loop
    return a.exec();
}
