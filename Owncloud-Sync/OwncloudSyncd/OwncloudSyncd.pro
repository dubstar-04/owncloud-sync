QT += core
QT -= gui

TARGET = OwncloudSyncd
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    owncloudsyncd.cpp

HEADERS += \
    owncloudsyncd.h

