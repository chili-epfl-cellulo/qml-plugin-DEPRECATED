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

    frameBuffer.reserve(IMG_WIDTH*IMG_HEIGHT);

    connected = false;
    connecting = false;

    timestampingEnabled = false;
    batteryState = 4; //Beginning with shutdown is a good idea
    x = 0;
    y = 0;
    theta = 0;
    lastTimestamp = 0;
    framerate = 0.0;
    kidnapped = true;
}

CelluloBluetooth::~CelluloBluetooth(){
}

void CelluloBluetooth::resetProperties(){

    sendPacket.clear();
    recvPacket.clear();

    timestampingEnabled = false;
    batteryState = 4; //Beginning with shutdown is a good idea
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
    while(frame.length() < IMG_WIDTH*IMG_HEIGHT)
        frame.append((int)0);
    return frame;
}



void CelluloBluetooth::setMacAddr(QString macAddr){
    disconnectFromServer();
    this->macAddr = macAddr;
    connectToServer();
}

void CelluloBluetooth::refreshConnection(){
    if(!connected){
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
        if(!connecting){
            connecting = true;
            emit connectingChanged();
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
        if(connected){
            connected = false;
            emit connectedChanged();
        }
        if(connecting){
            connecting = false;
            emit connectingChanged();
        }
        socket->deleteLater();
        socket = NULL;
        btConnectTimeoutTimer.stop();
        resetProperties();
    }
}

void CelluloBluetooth::socketConnected(){
    qDebug() << "CelluloBluetooth::socketConnected(): " << macAddr;
    if(!connected){
        connected = true;
        emit connectedChanged();
    }
    if(connecting){
        connecting = false;
        emit connectingChanged();
    }
}

void CelluloBluetooth::socketDisconnected(){
    qDebug() << "CelluloBluetooth::socketDisconnected(): " << macAddr << ", will try to reconnect.";
    if(connected){
        connected = false;
        emit connectedChanged();
    }
    if(connecting){
        connecting = false;
        emit connectingChanged();
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
        case RECEIVE_PACKET_TYPE::BOOT_COMPLETE:
            emit bootCompleted();
            break;

        case RECEIVE_PACKET_TYPE::WAKE_UP:
            emit wokeUp();
            break;

        case RECEIVE_PACKET_TYPE::SHUTTING_DOWN:
            emit shuttingDown();
            break;

        case RECEIVE_PACKET_TYPE::LOW_BATTERY:
            emit lowBattery();
            break;

        case RECEIVE_PACKET_TYPE::BATTERY_STATE_CHANGED: {
            int newState = recvPacket.unloadUInt8();
            if(newState >= 0 && newState <= 4)
                if(batteryState != newState){
                    batteryState = newState;
                    emit batteryStateChanged();
                }
            break;
        }

        case RECEIVE_PACKET_TYPE::TOUCH_BEGIN: {
            int key = recvPacket.unloadUInt8();
            if(key >= 0 && key <= 5)
                emit touchBegan(key);
            break;
        }

        case RECEIVE_PACKET_TYPE::TOUCH_LONG_PRESSED: {
            int key = recvPacket.unloadUInt8();
            if(key >= 0 && key <= 5)
                emit longTouch(key);
            break;
        }

        case RECEIVE_PACKET_TYPE::TOUCH_RELEASED: {
            int key = recvPacket.unloadUInt8();
            if(key >= 0 && key <= 5)
                emit touchReleased(key);
            break;
        }

        case RECEIVE_PACKET_TYPE::POSE_CHANGED: {
            x = recvPacket.unloadUInt32()/GOAL_POSE_FACTOR;
            y = recvPacket.unloadUInt32()/GOAL_POSE_FACTOR;
            theta = recvPacket.unloadUInt16()/GOAL_POSE_FACTOR;
            emit poseChanged();

            if(kidnapped){
                kidnapped = false;
                emit kidnappedChanged();
            }

            break;
        }

        case RECEIVE_PACKET_TYPE::POSE_CHANGED_TIMESTAMPED: {
            x = recvPacket.unloadUInt32()/GOAL_POSE_FACTOR;
            y = recvPacket.unloadUInt32()/GOAL_POSE_FACTOR;
            theta = recvPacket.unloadUInt16()/GOAL_POSE_FACTOR;
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

        case RECEIVE_PACKET_TYPE::KIDNAP: {
            int newKidnapped = recvPacket.unloadUInt8();
            if(newKidnapped == 0 || newKidnapped == 1)
                if((bool)newKidnapped != kidnapped){
                    kidnapped = (bool)newKidnapped;
                    emit kidnappedChanged();
                }
            break;
        }

        case RECEIVE_PACKET_TYPE::ACKNOWLEDGED:
            qDebug() << "CelluloBluetooth::processResponse(): Robot acknowledged";
            break;

        case RECEIVE_PACKET_TYPE::CAMERA_IMAGE_LINE: {
            quint16 line = recvPacket.unloadUInt16();

            //Drop previous incomplete frame
            if(frameBuffer.length() > line*IMG_WIDTH){
                qDebug() << "CelluloBluetooth::processResponse(): Dropping previously incomplete frame";
                frameBuffer.clear();
            }

            //Append possibly empty lines
            while(frameBuffer.length() < line*IMG_WIDTH){
                qDebug() << "CelluloBluetooth::processResponse(): Camera image line dropped";
                for(int i=0; i<IMG_WIDTH; i++)
                    frameBuffer.append('\0');
            }

            //Append line just received
            for(int i=0; i<IMG_WIDTH; i++)
                frameBuffer.append(recvPacket.unloadUInt8());

            //Update progress
            cameraImageProgress = (float)(line + 1)/IMG_HEIGHT;
            emit cameraImageProgressChanged();

            if(line >= IMG_HEIGHT - 1)
                emit frameReady();

            break;
        }

        case RECEIVE_PACKET_TYPE::DEBUG:
            qDebug() << "CelluloBluetooth::processResponse(): Debug message";
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
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::PING);
    sendCommand();
}

void CelluloBluetooth::setPoseBcastPeriod(unsigned int period){
    if(period > 0xFFFF)
        period = 0xFFFF;

    sendPacket.clear();
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_BCAST_PERIOD);
    sendPacket.load((quint16)period);
    sendCommand();
}

void CelluloBluetooth::setTimestampingEnabled(bool enabled){
    if(enabled != timestampingEnabled){
        timestampingEnabled = enabled;

        sendPacket.clear();
        sendPacket.setSendPacketType(SEND_PACKET_TYPE::TIMESTAMP_ENABLE);
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
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::FRAME_REQUEST);
    sendCommand();
}

void CelluloBluetooth::queryBatteryState(){
    sendPacket.clear();
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::BATTERY_STATE_REQUEST);
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
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_MOTOR_OUTPUT);
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
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_ALL_MOTOR_OUTPUTS);
    sendPacket.load((qint16)m1output);
    sendPacket.load((qint16)m2output);
    sendPacket.load((qint16)m3output);
    sendCommand();
}

void CelluloBluetooth::setGoalVelocity(float vx, float vy, float w){
    int vx_ = (int)(vx*GOAL_VELOCITY_FACTOR);
    int vy_ = (int)(vy*GOAL_VELOCITY_FACTOR);
    int w_ = (int)(w*GOAL_VELOCITY_FACTOR);

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
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_GOAL_VELOCITY);
    sendPacket.load((qint16)vx_);
    sendPacket.load((qint16)vy_);
    sendPacket.load((qint16)w_);
    sendCommand();
}

/*void CelluloBluetooth::setGoalVelocityCompact(int vx, int vy){
    int vx_ = vx/GOAL_VELOCITY_COMPACT_DIVISOR;
       int vy_ = vy/GOAL_VELOCITY_COMPACT_DIVISOR;

       if(vx_ < -0x80)
        vx_ = -0x80;
       else if(vx_ > 0x7F)
        vx_ = 0x7F;

       if(vy_ < -0x80)
        vy_ = -0x80;
       else if(vy_ > 0x7F)
        vy_ = 0x7F;

       QByteArray message;
       message = commandStrings[COMMAND_TYPE::SET_GOAL_VELOCITY_COMPACT];

       if(vx_ < 0){
        vx_ += 0x100;
       }
       message.append(getHexChar(vx_/0x10));
       message.append(getHexChar(vx_%0x10));

       if(vy_ < 0){
        vy_ += 0x100;
       }
       message.append(getHexChar(vy_/0x10));
       message.append(getHexChar(vy_%0x10));

       char checksum = getHexChar((getNumberOfOnes(vx_) + getNumberOfOnes(vy_)) % 16);
       message.append(checksum);

       message.append('\n');

       sendCommand(COMMAND_TYPE::SET_GOAL_VELOCITY_COMPACT, message);
   }
 */

void CelluloBluetooth::setGoalPose(float x, float y, float theta, float v, float w){
    x *= GOAL_POSE_FACTOR;
    y *= GOAL_POSE_FACTOR;
    theta *= GOAL_POSE_FACTOR;
    v *= GOAL_VELOCITY_FACTOR;
    w *= GOAL_VELOCITY_FACTOR;

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
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_GOAL_POSE);
    sendPacket.load((quint32)xi);
    sendPacket.load((quint32)yi);
    sendPacket.load((quint16)thetai);
    sendPacket.load((quint16)vi);
    sendPacket.load((quint16)wi);
    sendCommand();
}

void CelluloBluetooth::setGoalPosition(float x, float y, float v){
    x *= GOAL_POSE_FACTOR;
    y *= GOAL_POSE_FACTOR;
    v *= GOAL_VELOCITY_FACTOR;

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
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_GOAL_POSITION);
    sendPacket.load((quint32)xi);
    sendPacket.load((quint32)yi);
    sendPacket.load((quint16)vi);
    sendCommand();
}

void CelluloBluetooth::setVisualState(int state){
    sendPacket.clear();
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_VISUAL_STATE);
    sendPacket.load((quint8)state);
    sendCommand();
}

void CelluloBluetooth::setVisualEffect(int effect, QColor color, int value){
    if(value > 0xFF)
        value = 0xFF;
    else if(value < 0)
        value = 0;

    sendPacket.clear();
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SET_VISUAL_EFFECT);
    sendPacket.load((quint8)effect);
    sendPacket.load((quint8)color.red());
    sendPacket.load((quint8)color.green());
    sendPacket.load((quint8)color.blue());
    sendPacket.load((quint8)value);
    sendCommand();
}

void CelluloBluetooth::reset(){
    sendPacket.clear();
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::RESET);
    sendCommand();
}

void CelluloBluetooth::shutdown(){
    sendPacket.clear();
    sendPacket.setSendPacketType(SEND_PACKET_TYPE::SHUTDOWN);
    sendCommand();
}
