#-------------------------------------------------
#
# Project created by QtCreator 2018-01-31T19:04:43
#
#-------------------------------------------------


QT += core gui testlib network

CONFIG += c++11 -qt=qt5
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WTF_tests
TEMPLATE = app

INCLUDEPATH += include \
                $$PWD/../Engine/include

SOURCES += main.cpp \
           PageParserTest.cpp \
    PageLoaderTest.cpp

HEADERS  +=  \
           PageParserTest.h \
    PageLoaderTest.h

LIBS += -L$$OUT_PWD/../Engine -lwtf_engine

DEFINES += DATASET_DIR=\'\"$$PWD/dataset/\"\'
