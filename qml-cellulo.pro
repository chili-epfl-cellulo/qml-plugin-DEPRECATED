QT += qml quick bluetooth

CONFIG += qt c++11 nostrip

HEADERS += \
    src/CelluloBluetooth.h \
    src/CelluloBluetoothEnums.h \
    src/CameraFrameImageProvider.h \
    src/CelluloBluetoothPlugin.h \
    src/CelluloBluetoothPacket.h

SOURCES += \
    src/CelluloBluetooth.cpp \
    src/CelluloBluetoothEnums.cpp \
    src/CameraFrameImageProvider.cpp \
    src/CelluloBluetoothPlugin.cpp \
    src/CelluloBluetoothPacket.cpp

test{
    message(Configuring test build...)

    TEMPLATE = app
    TARGET = celluloplugintests

    QT += testlib

    HEADERS += \
        test/TestCelluloBluetoothPacket.h

    SOURCES += \
        test/TestCelluloBluetoothPacket.cpp
}
else{
    TEMPLATE = lib
    TARGET = celluloplugin

    CONFIG += plugin
    CONFIG -= android_install

    TARGET = $$qtLibraryTarget($$TARGET)
    uri = Cellulo

    #File installation

    qmldir.files = qmldir
    javascript.files = src/cellulo-toolkit.js
    qml.files = src/CelluloRobot.qml src/MacAddrSelector.qml

    OTHER_FILES += qmldir.files javascript.files qml.files

    unix {
        installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
        qmldir.path = $$installPath
        javascript.path = $$installPath
        qml.path = $$installPath
        target.path = $$installPath
        INSTALLS += target qmldir javascript qml
    }
}
