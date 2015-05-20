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

#include<QTimer>
#include<QQueue>
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

    enum COMMAND{
        PING = 0,
        FRAME_REQUEST,
        BATTERY_STATE_REQUEST,
        SET_VISUAL_STATE,
        SET_VISUAL_EFFECT,
        RESET,
        SHUTDOWN
    };

    enum RECEIVE_MESSAGES{
        BOOT_COMPLETE = 0,
        WAKE_UP,
        SHUTTING_DOWN,
        LOW_BATTERY,
        BATTERY_STATE_CHANGED,
        TOUCH_BEGIN,
        TOUCH_LONG_PRESSED,
        TOUCH_RELEASED,
        POSE_CHANGED,
        KIDNAP,
        ACKNOWLEDGED,
        NOT_ACKNOWLEDGED,
        NUM_RECEIVE_MESSAGES,
        INVALID_MESSAGE = -1
    };

    static const int COMMAND_TIMEOUT_MILLIS = 500;   ///< Will wait this many millis for a response before resending command

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

    /**
     * @brief Called when the server sends data
     */
    void socketDataArrived();

    /**
     * @brief Called when (re)connected to server
     */
    void socketConnected();

    /**
     * @brief Called when disconnected from server
     */
    void socketDisconnected();

    /**
     * @brief Called when server did not respond to command within the timeout interval
     */
    void serverTimeout();

public slots:

    /**
     * @brief Sets the MAC address of the server and immediately connects to it
     *
     * @param macAddr Bluetooth MAC address of the server (robot)
     */
    void setMacAddr(QString macAddr);

    /**
     * @brief Sends a ping, expecting an acknowledge
     */
    void ping();

private:

    static const char* commandStrings[];    ///< Strings sent over Bluetooth to give commands
    static const char* receiveStrings[];    ///< Strings received over Bluetooth as response or event

    QBluetoothSocket* socket;               ///< Bluetooth socket connected to the server
    QString macAddr;                        ///< Bluetooth MAC address of the server
    QQueue<QByteArray> commands;            ///< Commands to be sent over Bluetooth
    QTimer commandTimeout;                  ///< When this timer runs out, command is resent if not already acknowledged
    QByteArray receiveBuffer;               ///< Receive buffer until the current response/event message is complete

    /**
     * @brief Connects or reconnects to the server if not already connected
     */
    void reconnectToServer();

    /**
     * @brief Sends next command over Bluetooth socket and starts timeout timer
     */
    void sendCommand();

    /**
     * @brief Processes the response in the receive buffer if possible
     */
    void processResponse();

    /**
     * @brief Gets the ordinal of the received message in the receive buffer
     *
     * @return Received message ordinal in the RECEIVE_MESSAGES enum
     */
    RECEIVE_MESSAGES getReceivedMessage();
};

#endif // CELLULOBLUETOOTH_H
