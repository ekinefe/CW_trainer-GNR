QT += core gui widgets serialport multimedia
TARGET = CW_Trainer-GNR
TEMPLATE = app

SOURCES += src/main.cpp \
    src/MainWindow.cpp \
    src/CheatSheetWindow.cpp \
    src/SerialManager.cpp \
    src/SoundGenerator.cpp \
    src/StatisticsTracker.cpp \
    src/StatisticsWindow.cpp

HEADERS += src/MainWindow.h \
    src/CheatSheetWindow.h \
    src/MorseUtils.h \
    src/SerialManager.h \
    src/SoundGenerator.h \
    src/StatisticsTracker.h \
    src/StatisticsWindow.h
