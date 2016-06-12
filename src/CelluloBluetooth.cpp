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

#include <QBluetoothDeviceInfo>
#include <time.h>

QByteArray CelluloBluetooth::frameBuffer;

CelluloBluetooth::CelluloBluetooth(QQuickItem* parent) :
    QQuickItem(parent){
    socket = NULL;

    btConnectTimeoutTimer.setSingleShot(true);
    btConnectTimeoutTimer.setInterval(BT_CONNECT_TIMEOUT_MILLIS);
    connect(&btConnectTimeoutTimer, SIGNAL(timeout()), this, SLOT(refreshConnection()));

    //TODO: CALL RESET PROPERTIES INSTEAD OF THE BELOW

    frameBuffer.reserve(IMG_WIDTH_SHARED*IMG_HEIGHT_SHARED);

    connectionStatus = CelluloBluetoothEnums::ConnectionStatusDisconnected;

    timestampingEnabled = false;
    batteryState = CelluloBluetoothEnums::BatteryStateShutdown; //Beginning with shutdown is a good idea
    x = 0;
    y = 0;
    theta = 0;
    lastTimestamp = 0;
    framerate = 0.0;
    kidnapped = true;

    touchRawValues.reserve(6);
    for(int i=0;i<6;i++)
        touchRawValues.append(0);
}

CelluloBluetooth::~CelluloBluetooth(){
}

void CelluloBluetooth::resetProperties(){

    sendPacket.clear();
    recvPacket.clear();

    timestampingEnabled = false;
    batteryState = CelluloBluetoothEnums::BatteryStateShutdown; //Beginning with shutdown is a good idea
    emit batteryStateChanged();
    x = 0;
    y = 0;
    theta = 0;
    emit poseChanged();
    lastTimestamp = 0;
    framerate = 0.0;
    emit timestampChanged();
    kidnapped = true;
    emit kidnappedChanged();
    for(int i=0; i<6; i++)
        emit touchReleased(i);
}

QVariantList CelluloBluetooth::getFrame() const {
    QVariantList frame;
    for(int i=0; i<frameBuffer.length(); i++)
        frame.append((int)frameBuffer[i]);
    while(frame.length() < IMG_WIDTH_SHARED*IMG_HEIGHT_SHARED)
        frame.append((int)0);
    return frame;
}

void CelluloBluetooth::setMacAddr(QString macAddr){
    disconnectFromServer();
    this->macAddr = macAddr;
    connectToServer();
}

void CelluloBluetooth::refreshConnection(){
    if(connectionStatus != CelluloBluetoothEnums::ConnectionStatusConnected){
        qDebug() << "CelluloBluetooth::refreshConnection(): Connection attempt timed out, will retry";
        disconnectFromServer();
        connectToServer();
    }
}

void CelluloBluetooth::openSocket(){
    if(socket != NULL){
        qDebug() << "CelluloBluetooth::openSocket(): " << macAddr << "...";

        socket->connectToService(
            QBluetoothAddress(macAddr),
                #ifdef ANDROID
            QBluetoothUuid(QBluetoothUuid::SerialPort)
                #else
            1     //TODO: Temporary fix until https://bugreports.qt.io/browse/QTBUG-53041 is fixed
                #endif
            );
        btConnectTimeoutTimer.start();
        if(connectionStatus != CelluloBluetoothEnums::ConnectionStatusConnecting){
            connectionStatus = CelluloBluetoothEnums::ConnectionStatusConnecting;
            emit connectionStatusChanged();
        }
    }
}

void CelluloBluetooth::connectToServer(){
    if(socket == NULL){
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

        connect(socket, SIGNAL(readyRead()), this, SLOT(socketDataArrived()));
        connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
        connect(socket, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error),
                [=](QBluetoothSocket::SocketError error){ qDebug() << error; });

        openSocket();
    }
}

void CelluloBluetooth::disconnectFromServer(){
    if(socket != NULL){
        qDebug() << "CelluloBluetooth::disconnectFromServer(): " << macAddr << "...";
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(socketDataArrived()));
        disconnect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
        disconnect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
        socket->abort();
        socket->close();
        if(connectionStatus != CelluloBluetoothEnums::ConnectionStatusDisconnected){
            connectionStatus = CelluloBluetoothEnums::ConnectionStatusDisconnected;
            emit connectionStatusChanged();
        }
        socket->deleteLater();
        socket = NULL;
        btConnectTimeoutTimer.stop();
        resetProperties();
    }
}

void CelluloBluetooth::socketConnected(){
    qDebug() << "CelluloBluetooth::socketConnected(): " << macAddr;
    if(connectionStatus != CelluloBluetoothEnums::ConnectionStatusConnected){
        connectionStatus = CelluloBluetoothEnums::ConnectionStatusConnected;
        emit connectionStatusChanged();
    }

    //Update residual states that normally update by events
    queryBatteryState();
}

void CelluloBluetooth::socketDisconnected(){
    qDebug() << "CelluloBluetooth::socketDisconnected(): " << macAddr << ", will try to reconnect.";
    if(connectionStatus != CelluloBluetoothEnums::ConnectionStatusDisconnected){
        connectionStatus = CelluloBluetoothEnums::ConnectionStatusDisconnected;
        emit connectionStatusChanged();
    }
    openSocket();
}

void CelluloBluetooth::socketDataArrived(){
    QByteArray message = socket->readAll();

    for(int i=0; i<message.length(); i++)

        //Load byte and check end of packet
        if(recvPacket.loadReceivedByte(message[i]))
            processResponse();
}

void CelluloBluetooth::processResponse(){
    switch(recvPacket.getReceivePacketType()){
        case CelluloBluetoothPacket::EventPacketTypeBootComplete:
            emit bootCompleted();
            break;

        case CelluloBluetoothPacket::EventPacketTypeShuttingDown:
            emit shuttingDown();
            break;

        case CelluloBluetoothPacket::EventPacketTypeLowBattery:
            emit lowBattery();
            break;

        case CelluloBluetoothPacket::EventPacketTypeBatteryStateChanged: {
            CelluloBluetoothEnums::BatteryState newState = (CelluloBluetoothEnums::BatteryState)recvPacket.unloadUInt8();
            if(batteryState != newState){
                batteryState = newState;
                emit batteryStateChanged();
            }
            break;
        }

        case CelluloBluetoothPacket::EventPacketTypeTouchBegan: {
            int key = recvPacket.unloadUInt8();
            if(key >= 0 && key <= 5)
                emit touchBegan(key);
            break;
        }

        case CelluloBluetoothPacket::EventPacketTypeTouchLongPressed: {
            int key = recvPacket.unloadUInt8();
            if(key >= 0 && key <= 5)
                emit longTouch(key);
            break;
        }

        case CelluloBluetoothPacket::EventPacketTypeTouchReleased: {
            int key = recvPacket.unloadUInt8();
            if(key >= 0 && key <= 5)
                emit touchReleased(key);
            break;
        }

        case CelluloBluetoothPacket::EventPacketTypeTouchRaw: {
            for(int i=0;i<6;i++)
                touchRawValues[i] = recvPacket.unloadInt16();
            emit touchRawValuesUpdated();
            break;
        }

        case CelluloBluetoothPacket::EventPacketTypePoseChanged: {
            x = recvPacket.unloadUInt32()/(float)GOAL_POSE_FACTOR_SHARED;
            y = recvPacket.unloadUInt32()/(float)GOAL_POSE_FACTOR_SHARED;
            theta = recvPacket.unloadUInt16()/(float)GOAL_POSE_FACTOR_SHARED;
            emit poseChanged();

            if(kidnapped){
                kidnapped = false;
                emit kidnappedChanged();
            }

            break;
        }

        case CelluloBluetoothPacket::EventPacketTypePoseChangedTimestamped: {
            x = recvPacket.unloadUInt32()/(float)GOAL_POSE_FACTOR_SHARED;
            y = recvPacket.unloadUInt32()/(float)GOAL_POSE_FACTOR_SHARED;
            theta = recvPacket.unloadUInt16()/(float)GOAL_POSE_FACTOR_SHARED;
            emit poseChanged();

            unsigned int newTimestamp = recvPacket.unloadUInt32();
            framerate =
                FRAMERATE_SMOOTH_FACTOR*framerate +
                (1.0 - FRAMERATE_SMOOTH_FACTOR)*1000/(newTimestamp - lastTimestamp);
            lastTimestamp = newTimestamp;
            emit timestampChanged();

            if(kidnapped){
                kidnapped = false;
                emit kidnappedChanged();
            }

            break;
        }

        case CelluloBluetoothPacket::EventPacketTypeKidnapChanged: {
            int newKidnapped = recvPacket.unloadUInt8();
            if(newKidnapped == 0 || newKidnapped == 1)
                if((bool)newKidnapped != kidnapped){
                    kidnapped = (bool)newKidnapped;
                    emit kidnappedChanged();
                }
            break;
        }

        case CelluloBluetoothPacket::EventPacketTypeAcknowledged:
            qDebug() << "CelluloBluetooth::processResponse(): Robot acknowledged";
            break;

        case CelluloBluetoothPacket::EventPacketTypeFrameLine: {
            quint16 line = recvPacket.unloadUInt16();

            //Drop previous incomplete frame
            if(frameBuffer.length() > line*IMG_WIDTH_SHARED){
                qDebug() << "CelluloBluetooth::processResponse(): Dropping previously incomplete frame";
                frameBuffer.clear();
            }

            //Append possibly empty lines
            while(frameBuffer.length() < line*IMG_WIDTH_SHARED){
                qDebug() << "CelluloBluetooth::processResponse(): Camera image line dropped";
                for(int i=0; i<IMG_WIDTH_SHARED; i++)
                    frameBuffer.append('\0');
            }

            //Append line just received
            for(int i=0; i<IMG_WIDTH_SHARED; i++)
                frameBuffer.append(recvPacket.unloadUInt8());

            //Update progress
            cameraImageProgress = (float)(line + 1)/IMG_HEIGHT_SHARED;
            emit cameraImageProgressChanged();

            if(line >= IMG_HEIGHT_SHARED - 1)
                emit frameReady();

            break;
        }

        case CelluloBluetoothPacket::EventPacketTypeDebug:
            qDebug() << "CelluloBluetoothPacket::processResponse(): Debug event received";
            //DO SOMETHING
            break;

        default:
            break;
    }

    recvPacket.clear();
}

void CelluloBluetooth::sendCommand(){
    if(socket != NULL)
        socket->write(sendPacket.getSendData());
}

void CelluloBluetooth::ping(){
    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypePing);
    sendCommand();
}

void CelluloBluetooth::setPoseBcastPeriod(unsigned int period){
    if(period > 0xFFFF)
        period = 0xFFFF;

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetBcastPeriod);
    sendPacket.load((quint16)period);
    sendCommand();
}

void CelluloBluetooth::setTimestampingEnabled(bool enabled){
    if(enabled != timestampingEnabled){
        timestampingEnabled = enabled;

        sendPacket.clear();
        sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeTimestampEnable);
        sendPacket.load((quint8)enabled);

        sendCommand();
    }
}

void CelluloBluetooth::requestFrame(){
    frameBuffer.clear();
    if(cameraImageProgress != 0.0f){
        cameraImageProgress = 0.0f;
        emit cameraImageProgressChanged();
    }

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeFrameRequest);
    sendCommand();
}

void CelluloBluetooth::queryBatteryState(){
    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeBatteryStateRequest);
    sendCommand();
}

void CelluloBluetooth::setMotor1Output(int output){
    setMotorOutput(1, output);
}

void CelluloBluetooth::setMotor2Output(int output){
    setMotorOutput(2, output);
}

void CelluloBluetooth::setMotor3Output(int output){
    setMotorOutput(3, output);
}

void CelluloBluetooth::setMotorOutput(int motor, int output){
    if(motor != 1 && motor != 2 && motor != 3)
        return;

    if(output < -0xFFF)
        output = -0xFFF;
    else if(output > 0xFFF)
        output = 0xFFF;

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetMotorOutput);
    sendPacket.load((quint8)motor);
    sendPacket.load((qint16)output);
    sendCommand();
}

void CelluloBluetooth::setAllMotorOutputs(int m1output, int m2output, int m3output){
    if(m1output < -0xFFF)
        m1output = -0xFFF;
    else if(m1output > 0xFFF)
        m1output = 0xFFF;

    if(m2output < -0xFFF)
        m2output = -0xFFF;
    else if(m2output > 0xFFF)
        m2output = 0xFFF;

    if(m3output < -0xFFF)
        m3output = -0xFFF;
    else if(m3output > 0xFFF)
        m3output = 0xFFF;

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetAllMotorOutputs);
    sendPacket.load((qint16)m1output);
    sendPacket.load((qint16)m2output);
    sendPacket.load((qint16)m3output);
    sendCommand();
}

void CelluloBluetooth::setGoalVelocity(float vx, float vy, float w){
    int vx_ = (int)(vx*GOAL_VEL_FACTOR_SHARED);
    int vy_ = (int)(vy*GOAL_VEL_FACTOR_SHARED);
    int w_ = (int)(w*GOAL_VEL_FACTOR_SHARED);

    if(vx_ < -0x7FFF)
        vx_ = -0x7FFF;
    else if(vx_ > 0x7FFF)
        vx_ = 0x7FFF;

    if(vy_ < -0x7FFF)
        vy_ = -0x7FFF;
    else if(vy_ > 0x7FFF)
        vy_ = 0x7FFF;

    if(w_ < -0x7FFF)
        w_ = -0x7FFF;
    else if(w_ > 0x7FFF)
        w_ = 0x7FFF;

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetGoalVelocity);
    sendPacket.load((qint16)vx_);
    sendPacket.load((qint16)vy_);
    sendPacket.load((qint16)w_);
    sendCommand();
}

void CelluloBluetooth::setGoalPose(float x, float y, float theta, float v, float w){
    x *= GOAL_POSE_FACTOR_SHARED;
    y *= GOAL_POSE_FACTOR_SHARED;
    theta *= GOAL_POSE_FACTOR_SHARED;
    v *= GOAL_VEL_FACTOR_SHARED;
    w *= GOAL_VEL_FACTOR_SHARED;

    quint32 xi, yi;
    quint16 thetai, vi, wi;

    if(x > (float)0xFFFFFFFF)
        xi = 0xFFFFFFFF;
    else
        xi = (quint32)x;

    if(y > (float)0xFFFFFFFF)
        yi = 0xFFFFFFFF;
    else
        yi = (quint32)y;

    if(theta > (float)0xFFFF)
        thetai = 0xFFFF;
    else
        thetai = (quint16)theta;

    if(v > (float)0xFFFF)
        vi = 0xFFFF;
    else
        vi = (quint16)v;

    if(w > (float)0xFFFF)
        wi = 0xFFFF;
    else
        wi = (quint16)w;

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetGoalPose);
    sendPacket.load((quint32)xi);
    sendPacket.load((quint32)yi);
    sendPacket.load((quint16)thetai);
    sendPacket.load((quint16)vi);
    sendPacket.load((quint16)wi);
    sendCommand();
}

void CelluloBluetooth::setGoalPosition(float x, float y, float v){
    x *= GOAL_POSE_FACTOR_SHARED;
    y *= GOAL_POSE_FACTOR_SHARED;
    v *= GOAL_VEL_FACTOR_SHARED;

    quint32 xi, yi;
    quint16 vi;

    if(x > (float)0xFFFFFFFF)
        xi = 0xFFFFFFFF;
    else
        xi = (quint32)x;

    if(y > (float)0xFFFFFFFF)
        yi = 0xFFFFFFFF;
    else
        yi = (quint32)y;

    if(v > (float)0xFFFF)
        vi = 0xFFFF;
    else
        vi = (quint16)v;

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetGoalPosition);
    sendPacket.load((quint32)xi);
    sendPacket.load((quint32)yi);
    sendPacket.load((quint16)vi);
    sendCommand();
}

void CelluloBluetooth::setLEDResponseMode(CelluloBluetoothEnums::LEDResponseMode mode){
    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetLEDResponseMode);
    sendPacket.load((quint8)mode);
    sendCommand();
}

void CelluloBluetooth::setLocomotionInteractivityMode(CelluloBluetoothEnums::LocomotionInteractivityMode mode){
    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetLocomotionInteractivityMode);
    sendPacket.load((quint8)mode);
    sendCommand();
}

void CelluloBluetooth::setRawTouchEnabled(bool enabled){
    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeRawTouchEnable);
    sendPacket.load((quint8)enabled);
    sendCommand();
}

void CelluloBluetooth::setVisualEffect(CelluloBluetoothEnums::VisualEffect effect, QColor color, int value){
    if(value > 0xFF)
        value = 0xFF;
    else if(value < 0)
        value = 0;

    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeSetVisualEffect);
    sendPacket.load((quint8)effect);
    sendPacket.load((quint8)color.red());
    sendPacket.load((quint8)color.green());
    sendPacket.load((quint8)color.blue());
    sendPacket.load((quint8)value);
    sendCommand();
}

void CelluloBluetooth::reset(){
    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeReset);
    sendCommand();
}

void CelluloBluetooth::shutdown(){
    sendPacket.clear();
    sendPacket.setSendPacketType(CelluloBluetoothPacket::CmdPacketTypeShutdown);
    sendCommand();
}
