QT += core sql
QT -= gui

TARGET = OwncloudSyncd
CONFIG += console
CONFIG -= app_bundle

load(ubuntu-click)

TEMPLATE = app

SOURCES += main.cpp \
    owncloudsyncd.cpp

HEADERS += \
    owncloudsyncd.h










# Default rules for deployment.
target.path = $${UBUNTU_CLICK_BINARY_PATH}
INSTALLS+=target
