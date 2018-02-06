#-------------------------------------------------
#
# Project created by QtCreator 2018-01-31T19:04:43
#
#-------------------------------------------------

QT += core gui \
   network
TEMPLATE = lib

CONFIG += c++11 -qt=qt5
CONFIG += sharedlib


TARGET = wtf_engine

INCLUDEPATH += include

SOURCES += src/PageParser.cpp \
           src/PageLoader.cpp \
           src/Manager.cpp

HEADERS  += include/PageParser.h \
            include/PageLoader.h \
            include/Manager.h

