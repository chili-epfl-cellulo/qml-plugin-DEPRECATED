/*
 * Copyright (C) 2015 EPFL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

/**
 * @file CelluloBluetoothPlugin.cpp
 * @brief Source for exposing Cellulo Bluetooth communication as a QML object
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2015-05-20
 */

#include "CelluloBluetoothPlugin.h"

#include<QQmlEngine>

#include"CameraFrameImageProvider.h"

void CelluloBluetoothPlugin::registerTypes(const char *uri){
    qmlRegisterType<CelluloBluetooth>(uri, 1, 0, "CelluloBluetooth");
}

void CelluloBluetoothPlugin::initializeEngine(QQmlEngine *engine, const char *uri __attribute__((unused))){
    engine->addImageProvider("cameraFrame", new CameraFrameImageProvider());
}
