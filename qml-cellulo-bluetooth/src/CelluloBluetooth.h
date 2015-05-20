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
 * @file CelluloBluetooth.h
 * @brief Header for Bluetooth communication with Cellulo robots
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2015-05-20
 */

#ifndef CELLULOBLUETOOTH_H
#define CELLULOBLUETOOTH_H

#include<QQuickItem>
#include<QBluetoothSocket>

/**
 * @brief Bluetooth communicator for a Cellulo robot
 */
class CelluloBluetooth : public QQuickItem {
Q_OBJECT
    Q_PROPERTY(QString macAddr WRITE setMacAddr)
    //Q_PROPERTY(DetectionTrigger detectionTrigger WRITE setDetectionTrigger)
    //Q_PROPERTY(QVariant sourceImage WRITE setSourceImage)
    //Q_PROPERTY(QVariantMap tags READ getTags NOTIFY tagsChanged) //TODO: would a QMap<QString,QMatrix> do ?
    //Q_PROPERTY(QMatrix4x4 projectionMatrix READ getProjectionMatrix NOTIFY projectionMatrixChanged)
    //Q_PROPERTY(QString tagConfigurationFile WRITE setTagConfigurationFile)
    //Q_PROPERTY(QObject* imu WRITE setIMU)
    //Q_PROPERTY(qreal persistence WRITE setPersistence)

public:

    /**
     * @brief Creates a new Cellulo robot communicator
     *
     * @param parent Parent of this QML item
     */
    explicit CelluloBluetooth(QQuickItem* parent = 0);

    /**
     * @brief Destroys this Cellulo robot communicator
     */
    virtual ~CelluloBluetooth();

private slots:

    void socketDataArrived();

    void socketConnected();

    void socketDisconnected();

public slots:

    /**
     * @brief Sets the MAC address of the server and immediately connects to it
     *
     * @param macAddr Bluetooth MAC address of the server (robot)
     */
    void setMacAddr(QString macAddr);

signals:


private:

    QBluetoothSocket* socket;   ///< Bluetooth socket connected to the server
    QString macAddr;            ///< Bluetooth MAC address of the server

    /**
     * @brief Connects or reconnects to the server if not already connected
     */
    void reconnectToServer();

};

#endif // CELLULOBLUETOOTH_H
