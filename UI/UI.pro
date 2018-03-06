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

INCLUDEPATH += include \
                $$PWD/../Engine/include

SOURCES += main.cpp \
        mainwindow.cpp 

HEADERS  +=  \
    mainwindow.h \

FORMS    += mainwindow.ui

LIBS += -L$$OUT_PWD/../Engine -lwtf_engine

RESOURCES += \
    resourses.qrc
