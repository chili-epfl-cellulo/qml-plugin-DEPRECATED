QT += testlib

TEMPLATE = app
TARGET = celluloplugintests

CONFIG += qt c++11 nostrip

HEADERS += \
    ../src/CelluloBluetoothPacket.h \
    TestCelluloBluetoothPacket.h

SOURCES += \
    ../src/CelluloBluetoothPacket.cpp \
    TestCelluloBluetoothPacket.cpp
