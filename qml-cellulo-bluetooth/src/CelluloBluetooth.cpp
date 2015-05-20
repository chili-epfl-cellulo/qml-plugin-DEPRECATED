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
 * @file CelluloBluetooth.cpp
 * @brief Source for Bluetooth communication with Cellulo robots
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2015-05-20
 */

#include "CelluloBluetooth.h"

#include<QBluetoothDeviceInfo>

CelluloBluetooth::CelluloBluetooth(QQuickItem* parent) :
    QQuickItem(parent)
{ }

CelluloBluetooth::~CelluloBluetooth(){ }

void CelluloBluetooth::setMacAddr(QString macAddr){
    QBluetoothServiceInfo service;
    service.setServiceUuid(QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB"))); //Serial port profile
    service.setDevice(QBluetoothDeviceInfo(QBluetoothAddress(QString("00:06:66:74:41:04")),QString(""),0));

    qDebug("CONNECTING");

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    socket->connectToService(service);

    connect(socket, SIGNAL(readyRead()), this, SLOT(socketDataArrived()));
    connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
}

void CelluloBluetooth::socketDataArrived(){
    qDebug() << socket->readAll();
}

void CelluloBluetooth::socketConnected(){
    qDebug() << "CONNECTED";
}

void CelluloBluetooth::socketDisconnected(){
    qDebug() << "DISCONNECTED";
}

