#-------------------------------------------------
#
# Project created by QtCreator 2014-05-07T22:53:27
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Evolve
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    poly.cpp \
    progressdialog.cpp \
    settingswidget.cpp \
    settings.cpp \
    widgetGui.cpp \
    mutation.cpp \
    optimize.cpp \
    analysis.cpp

HEADERS  += widget.h \
    poly.h \
    progressdialog.h \
    settingswidget.h \
    settings.h \
    analysis.h

FORMS    += widget.ui \
    progressdialog.ui \
    settingswidget.ui

QMAKE_CXXFLAGS_RELEASE += -std=c++11 -O3 -flto -march=native -mmmx # Crashes on some computers
QMAKE_CXXFLAGS_DEBUG += -std=c++11 -Og -g -march=native -mmmx # Crashes on some computers
#QMAKE_CXXFLAGS_DEBUG += -Og
