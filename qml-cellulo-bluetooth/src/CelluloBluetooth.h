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

    /**
     * @brief Gets the detected tags
     *
     * @return Most recent detected tags
     */
  //  QVariantMap getTags() const;

    /**
     * @brief Gets the transform matrix that projects the pose onto the camera image
     *
     * @return Transform matrix that project the pose onto the camera image
     */
//    QMatrix4x4 getProjectionMatrix() const;

    /**
     * @brief Sets the detection trigger
     *
     * @param trigger The new detection trigger
     */
//    void setDetectionTrigger(DetectionTrigger trigger);

    /**
     * @brief Pushes a new image to be processed
     *
     * @param sourceImage Must contain a cv::Mat that is e.g the latest camera image
     */
//    void setSourceImage(QVariant sourceImage);

    /**
     * @brief Reads a Chilitags tag configuration from the given qrc file
     *
     * @param tagConfigurationFile The qrc file, must begin with :/
     */
//    void setTagConfigurationFile(QString tagConfigurationFile);

    /**
     * @brief Sets the IMU object that has displacement getter/resetter callbacks
     *
     * @param imu Imu object of type IMU
     */
//    void setIMU(QObject* imu);

    /**
     * @brief Sets the persistence of tags against being discarded when not detected for a while
     *
     * @param persistence Roughly corresponds to number of undetected frames
     */
//    void setPersistence(qreal persistence);

signals:

    /**
     * @brief Emitted when a new image is processed
     *
     * @param tags The new set of tags
     */
//    void tagsChanged(QVariantMap tags);

//    void projectionMatrixChanged();

private:
    QBluetoothSocket* socket; ///< Bluetooth socket connected to the server

};

#endif // CELLULOBLUETOOTH_H
