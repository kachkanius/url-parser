#-------------------------------------------------
#
# Project created by QtCreator 2018-01-31T19:04:43
#
#-------------------------------------------------

QT += core gui \
   network

CONFIG += c++11 -qt=qt5
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WebTextFinder
TEMPLATE = app

INCLUDEPATH += include

SOURCES += main.cpp \
        mainwindow.cpp \
        src/PageParser.cpp \
        src/PageLoader.cpp

HEADERS  +=  \
    mainwindow.h \
    include/PageParser.h \
    include/PageLoader.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    CMakeLists.txt
