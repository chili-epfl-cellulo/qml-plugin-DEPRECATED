#Building of native code

TEMPLATE = lib
TARGET = celluloplugin

CONFIG += qt plugin c++11 nostrip
CONFIG -= android_install

QT += qml quick bluetooth

TARGET = $$qtLibraryTarget($$TARGET)
uri = Cellulo

HEADERS += \
    src/CelluloBluetooth.h \
    src/CameraFrameImageProvider.h \
    src/CelluloBluetoothPlugin.h

SOURCES += \
    src/CelluloBluetooth.cpp \
    src/CameraFrameImageProvider.cpp \
    src/CelluloBluetoothPlugin.cpp

#File installation

qmldir.files = qmldir
javascript.files = src/cellulo-toolkit.js
qml.files = src/CelluloRobot.qml

OTHER_FILES += qmldir.files javascript.files qml.files

unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    javascript.path = $$installPath
    qml.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir javascript qml
}

