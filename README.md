qml-cellulo-bluetooth
=====================

This is a qml module for communicating with a Cellulo robot over bluetooth. It
supports desktop and Android and was tested with Qt 5.3.2.

Desktop build
-------------

Tested on Ubuntu 14.04:

```
mkdir build-desktop && cd build-desktop
/path-to-qt-install-root/5.3/gcc_64/bin/qmake ..
make -j5 install
```

Android build
-------------

Tested with Android API 14 (Android version 4.0) and Android NDK r9d:

```
export ANDROID_NDK_ROOT=/path-to-android-ndk/
```

```
mkdir build-android && cd build-android
/path-to-qt-install-root/5.3/android_armv7/bin/qmake ..
make -j5 install
```

