TEMPLATE = lib
TARGET = cellulobluetoothplugin

CONFIG += qt plugin c++11 nostrip
CONFIG -= android_install

QT += qml quick bluetooth

TARGET = $$qtLibraryTarget($$TARGET)
uri = CelluloBluetooth

HEADERS += \
    src/CelluloBluetooth.h \
    src/CelluloBluetoothPlugin.h

SOURCES += \
    src/CelluloBluetooth.cpp \
    src/CelluloBluetoothPlugin.cpp

OTHER_FILES += qmldir

#Install plugin library, qmldir
qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

