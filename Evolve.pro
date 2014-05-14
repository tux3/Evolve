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
    stats.cpp

HEADERS  += widget.h \
    poly.h \
    progressdialog.h \
    settingswidget.h \
    settings.h \
    stats.h

FORMS    += widget.ui \
    progressdialog.ui \
    settingswidget.ui

CONFIG += c++11

QMAKE_CXXFLAGS_RELEASE += -O3 -flto -march=native # Crashes on some computers
QMAKE_CXXFLAGS_DEBUG += -O0
