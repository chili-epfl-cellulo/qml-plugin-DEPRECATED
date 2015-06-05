TEMPLATE = app

QT += qml quick bluetooth

SOURCES += src/main.cpp

RESOURCES += qml.qrc

android {
    target.path = /libs/armeabi-v7a
    export(target.path)
    INSTALLS += target
    export(INSTALLS)

    ANDROID_EXTRA_LIBS = \
        $$[QT_INSTALL_QML]/CelluloBluetooth/libcellulobluetoothplugin.so

    ANDROID_PERMISSIONS += \
        android.permission.BLUETOOTH
}
