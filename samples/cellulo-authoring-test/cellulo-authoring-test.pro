TEMPLATE = app
TARGET = celluloplugin

CONFIG += qt plugin
QT += qml quick

SOURCES += src/main.cpp

RESOURCES += qml.qrc


DISTFILES += \
    dummyMain.qml

android {
    target.path = /libs/armeabi-v7a
    export(target.path)
    INSTALLS += target
    export(INSTALLS)

    ANDROID_EXTRA_LIBS = \
        $$[QT_INSTALL_QML]/Cellulo/libcelluloplugin.so

    ANDROID_PERMISSIONS += \
        android.permission.BLUETOOTH
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat



ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

