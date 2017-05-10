#-------------------------------------------------
#
# Project created by QtCreator 2017-05-09T20:18:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GuiClient
TEMPLATE = app


SOURCES += main.cpp\
	client.cpp \
    bigpic.cpp

HEADERS  += client.h \
    bigpic.h

FORMS    += client.ui \
    bigpic.ui
