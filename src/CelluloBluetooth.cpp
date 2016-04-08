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
#include<time.h>

const char* CelluloBluetooth::commandStrings[] = {
    "P", //(P)ing
    "D", //Set broadcast perio(d)
    "I", //Enable (i)mage streaming + disable localization or vice-versa
    "T", //Enable (t)imestamping along with pose and disable idling or vice-versa
    "F", //Request (f)rame
    "B", //Query (b)attery state
    "V", //Set (v)isual state
    "E", //Set (e)ffect
    "M", //Set (m)otor output
    "A", //Set (a)ll motor outputs
    "C", //Set goal velo(c)ity
    "G", //Set (g)oal pose
    "R", //(R)eset
    "S"  //(S)hutdown
};

const char* CelluloBluetooth::receiveStrings[] = {
    "O", //B(o)ot finished
    "H", //Wake up finished ((H)ello)
    "D", //All keys long touched; shutting (d)own
    "W", //Battery lo(w), shutting down
    "B", //(B)attery state changed
    "T", //(T)ouch began
    "L", //(L)ong touch
    "R", //Touch (r)eleased
    "P", //(P)ose changed
    "K", //Robot (k)idnapped or unkidnapped
    "A", //(A)cknowledged
    "E"  //D(e)bug message
};

QByteArray CelluloBluetooth::frameBuffer;

CelluloBluetooth::CelluloBluetooth(QQuickItem* parent) :
    QQuickItem(parent)
{
    socket = NULL;

    btConnectTimeoutTimer.setSingleShot(true);
    btConnectTimeoutTimer.setInterval(BT_CONNECT_TIMEOUT_MILLIS);
    connect(&btConnectTimeoutTimer, SIGNAL(timeout()), this, SLOT(refreshConnection()));

    frameTimeoutTimer.setSingleShot(true);
    connect(&frameTimeoutTimer, SIGNAL(timeout()), this, SLOT(frameTimeout()));

    expectingFrame = false;
    frameBuffer.reserve(IMG_WIDTH*IMG_HEIGHT);

    connected = false;
    connecting = false;

    imageStreamingEnabled = false;
    timestampingEnabled = false;
    batteryState = 4; //Beginning with shutdown is a good idea
    x = 0;
    y = 0;
    theta = 0;
    lastTimestamp = 0;
    framerate = 0.0;
    kidnapped = true;
}

CelluloBluetooth::~CelluloBluetooth(){ }

void CelluloBluetooth::resetProperties(){
    expectingFrame = false;
    imageStreamingEnabled = false;
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
    for(int i=0;i<6;i++)
        emit touchReleased(i);
}

QVariantList CelluloBluetooth::getFrame() const {
    QVariantList frame;
    for(int i=0;i<frameBuffer.length();i++)
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
        qDebug() << "Connection attempt timed out, will retry.";
        disconnectFromServer();
        connectToServer();
    }
}

void CelluloBluetooth::openSocket(){
    if(socket != NULL){
        qDebug() << "Connecting to " << macAddr << "...";

        socket->connectToService(
                QBluetoothAddress(macAddr),
                QBluetoothUuid(QString("00000003-0000-1000-8000-00805F9B34FB"))); //Connect to the RFCOMM protocol
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

        openSocket();
    }
}

void CelluloBluetooth::disconnectFromServer(){
    if(socket != NULL){
        qDebug() << "Disconnecting from " << macAddr << "...";
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
    qDebug() << "Connected to " << macAddr << ".";
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
    qDebug() << "Disconnected from " << macAddr << ", will try to reconnect.";
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

    for(int i=0;i<message.length();i++){

        //We're receiving a camera image
        if(expectingFrame){
            frameBuffer.append(message[i]);
            currentPixel++;

            qDebug() << macAddr << " sent " << 100.0*currentPixel/(IMG_HEIGHT*IMG_WIDTH) << "% of frame";
            if(currentPixel >= IMG_HEIGHT*IMG_WIDTH){
                expectingFrame = false;
                frameTimeoutTimer.stop();
                qDebug() << macAddr << " sent complete frame";
                emit frameReady();
            }
        }

        //We're receiving regular message
        else{
            //End of transmission
            if(message[i] == '\n')
                processResponse();

            //Transmission continues
            else{
                receiveBuffer.append(message[i]);
            }
        }
    }
}

void CelluloBluetooth::processResponse(){
    //qDebug() << macAddr << " has sent: " << receiveBuffer;

    switch(getReceivedMessage()){
        case BOOT_COMPLETE:
            emit bootCompleted();
            break;

        case WAKE_UP:
            emit wokeUp();
            break;

        case SHUTTING_DOWN:
            emit shuttingDown();
            break;

        case LOW_BATTERY:
            emit lowBattery();
            break;

        case BATTERY_STATE_CHANGED:
            {
                int newState = (int)(receiveBuffer[1] - 48);
                if(newState >= 0 && newState <= 4){
                    if(batteryState != newState){
                        batteryState = newState;
                        emit batteryStateChanged();
                    }
                }
                break;
            }

        case TOUCH_BEGIN:
            {
                int key = (int)(receiveBuffer[1] - 48);
                if(key >= 0 && key <= 5)
                    emit touchBegan(key);
                break;
            }

        case TOUCH_LONG_PRESSED:
            {
                int key = (int)(receiveBuffer[1] - 48);
                if(key >= 0 && key <= 5)
                    emit longTouch(key);
                break;
            }

        case TOUCH_RELEASED:
            {
                int key = (int)(receiveBuffer[1] - 48);
                if(key >= 0 && key <= 5)
                    emit touchReleased(key);
                break;
            }

        case POSE_CHANGED:
            {
                x = hexToInt(receiveBuffer, 1, 8)/100.0f;
                y = hexToInt(receiveBuffer, 9, 16)/100.0f;
                theta = hexToInt(receiveBuffer, 17, 20)/100.0f;
                emit poseChanged();

                if(timestampingEnabled){
                    int newTimestamp = hexToInt(receiveBuffer, 21, 28);
                    framerate = FRAMERATE_SMOOTH_FACTOR*framerate + (1.0 - FRAMERATE_SMOOTH_FACTOR)*1000/(newTimestamp - lastTimestamp);
                    lastTimestamp = newTimestamp;
                    emit timestampChanged();
                }

                if(kidnapped){
                    kidnapped = false;
                    emit kidnappedChanged();
                }

                break;
            }

        case KIDNAP:
            {
                int newKidnapped = (int)(receiveBuffer[1] - 48);
                if(newKidnapped == 0 || newKidnapped == 1)
                    if((bool)newKidnapped != kidnapped){
                        kidnapped = newKidnapped;
                        emit kidnappedChanged();
                    }
                break;
            }

        case ACKNOWLEDGED:
            break;

        case DEBUG:
            qDebug() << "Debug message: " << receiveBuffer.right(receiveBuffer.size() - 1);
            break;

        default:
            break;
    }

    receiveBuffer.clear();
}

CelluloBluetooth::RECEIVE_MESSAGES CelluloBluetooth::getReceivedMessage(){
    for(int i=0;i<(int)RECEIVE_MESSAGES::NUM_RECEIVE_MESSAGES;i++)
        if(receiveStrings[i][0] == receiveBuffer[0])
            return (RECEIVE_MESSAGES)i;
    return RECEIVE_MESSAGES::INVALID_MESSAGE;
}

void CelluloBluetooth::frameTimeout(){
    qDebug() << macAddr << " timed out in sending the camera frame";
    expectingFrame = false;
    emit frameReady();
}

void CelluloBluetooth::sendCommand(COMMAND_TYPE type, QByteArray& message){
    if(!expectingFrame && socket != NULL){
        //qDebug() << "Sending command to " << macAddr << ": " << commands.head().message;

        if(type == COMMAND_TYPE::FRAME_REQUEST){
            frameTimeoutTimer.start(FRAME_TIMEOUT_MILLIS);
            expectingFrame = true;
            currentPixel = 0;
            frameBuffer.clear();
        }

        socket->write(message);
    }
}

void CelluloBluetooth::ping(){
    QByteArray message;
    message = commandStrings[COMMAND_TYPE::PING];
    message.append('\n');
    sendCommand(COMMAND_TYPE::PING, message);
}

void CelluloBluetooth::setPoseBcastPeriod(unsigned int period){
    if(period > 0xFFF)
        period = 0xFFF;

    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SET_BCAST_PERIOD];
    message.append(getHexChar(period/0x100));
    message.append(getHexChar(period/0x10%0x10));
    message.append(getHexChar(period%0x10));
    message.append('\n');
    sendCommand(COMMAND_TYPE::SET_BCAST_PERIOD, message);
}


void CelluloBluetooth::setImageStreamingEnabled(bool enabled){
    if(enabled != imageStreamingEnabled){
        imageStreamingEnabled = enabled;
        QByteArray message;
        message = commandStrings[COMMAND_TYPE::IMAGE_STREAM_ENABLE];
        message.append(enabled ? '1' : '0');
        message.append('\n');
        sendCommand(COMMAND_TYPE::IMAGE_STREAM_ENABLE, message);
    }
}

void CelluloBluetooth::setTimestampingEnabled(bool enabled){
    if(enabled != timestampingEnabled){
        timestampingEnabled = enabled;
        QByteArray message;
        message = commandStrings[COMMAND_TYPE::TIMESTAMP_ENABLE];
        message.append(enabled ? '1' : '0');
        message.append('\n');
        sendCommand(COMMAND_TYPE::TIMESTAMP_ENABLE, message);
    }
}

void CelluloBluetooth::requestFrame(){
    QByteArray message;
    message = commandStrings[COMMAND_TYPE::FRAME_REQUEST];
    message.append('\n');
    sendCommand(COMMAND_TYPE::FRAME_REQUEST, message);
}

void CelluloBluetooth::queryBatteryState(){
    QByteArray message;
    message = commandStrings[COMMAND_TYPE::BATTERY_STATE_REQUEST];
    message.append('\n');
    sendCommand(COMMAND_TYPE::BATTERY_STATE_REQUEST, message);
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

    bool negative = false;
    if(output < 0){
        output *= -1;
        negative = true;
    }

    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SET_MOTOR_OUTPUT];
    message.append((char)(motor + '0'));
    if(negative)
        message.append('-');
    message.append(getHexChar(output/0x100));
    message.append(getHexChar(output/0x10%0x10));
    message.append(getHexChar(output%0x10));
    message.append('\n');
    sendCommand(COMMAND_TYPE::SET_MOTOR_OUTPUT, message);
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

    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SET_ALL_MOTOR_OUTPUTS];

    if(m1output < 0){
        m1output *= -1;
        message.append('-');
    }
    message.append(getHexChar(m1output/0x100));
    message.append(getHexChar(m1output/0x10%0x10));
    message.append(getHexChar(m1output%0x10));

    if(m2output < 0){
        m2output *= -1;
        message.append('-');
    }
    message.append(getHexChar(m2output/0x100));
    message.append(getHexChar(m2output/0x10%0x10));
    message.append(getHexChar(m2output%0x10));

    if(m3output < 0){
        m3output *= -1;
        message.append('-');
    }
    message.append(getHexChar(m3output/0x100));
    message.append(getHexChar(m3output/0x10%0x10));
    message.append(getHexChar(m3output%0x10));

    message.append('\n');
    sendCommand(COMMAND_TYPE::SET_ALL_MOTOR_OUTPUTS, message);
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

    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SET_GOAL_VELOCITY];

    if(vx_ < 0){
        vx_ *= -1;
        message.append('-');
    }
    message.append(getHexChar(vx_/0x1000));
    message.append(getHexChar(vx_/0x100%0x10));
    message.append(getHexChar(vx_/0x10%0x10));
    message.append(getHexChar(vx_%0x10));

    if(vy_ < 0){
        vy_ *= -1;
        message.append('-');
    }
    message.append(getHexChar(vy_/0x1000));
    message.append(getHexChar(vy_/0x100%0x10));
    message.append(getHexChar(vy_/0x10%0x10));
    message.append(getHexChar(vy_%0x10));

     if(w_ < 0){
        w_ *= -1;
        message.append('-');
    }
    message.append(getHexChar(w_/0x1000));
    message.append(getHexChar(w_/0x100%0x10));
    message.append(getHexChar(w_/0x10%0x10));
    message.append(getHexChar(w_%0x10));

    message.append('\n');

    sendCommand(COMMAND_TYPE::SET_GOAL_VELOCITY, message);
}

void CelluloBluetooth::setGoalPose(float x, float y, float theta, float v, float w){
    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SET_GOAL_POSE];
    static char buf[31 + 1];
    sprintf(buf,"%08X%08X%04X%04X%04X",
            (unsigned int)(GOAL_POSE_FACTOR*x), (unsigned int)(GOAL_POSE_FACTOR*y), (unsigned int)(GOAL_POSE_FACTOR*theta),
            (unsigned int)(GOAL_VELOCITY_FACTOR*v), (unsigned int)(GOAL_VELOCITY_FACTOR*w));
    message.append(buf);
    message.append('\n');
    sendCommand(COMMAND_TYPE::SET_GOAL_POSE, message);
}

void CelluloBluetooth::setVisualState(int state){
    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SET_VISUAL_STATE];
    message.append((char)(state + 48));
    message.append('\n');
    sendCommand(COMMAND_TYPE::SET_VISUAL_STATE, message);
}

void CelluloBluetooth::setVisualEffect(int effect, QColor color, int value){
    if(value > 255)
        value = 255;
    else if(value < 0)
        value = 0;

    QString colorName = color.name(QColor::NameFormat::HexArgb); //In the form '#aarrggbb'
    colorName = colorName.toUpper();
    colorName.remove(0,3); //Remove # and alpha
    QString valueString;
    valueString.append((char)(value/16 >= 10 ? value/16 + 65 - 10 : value/16 + 48));
    valueString.append((char)(value%16 >= 10 ? value%16 + 65 - 10 : value%16 + 48));

    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SET_VISUAL_EFFECT];
    message.append((char)(effect + 48));
    message.append(colorName);
    message.append(valueString);
    message.append('\n');
    sendCommand(COMMAND_TYPE::SET_VISUAL_EFFECT, message);
}

void CelluloBluetooth::reset(){
    QByteArray message;
    message = commandStrings[COMMAND_TYPE::RESET];
    message.append('\n');
    sendCommand(COMMAND_TYPE::RESET, message);
}

void CelluloBluetooth::shutdown(){
    QByteArray message;
    message = commandStrings[COMMAND_TYPE::SHUTDOWN];
    message.append('\n');
    sendCommand(COMMAND_TYPE::SHUTDOWN, message);
}

int CelluloBluetooth::hexToInt(QByteArray const& array, int begin, int end){
    int result = 0;
    for(int i=begin;i<=end;i++){
        result <<= 4;
        result += (int)array[i] >= 65 ? (int)array[i] - 65 + 10 : (int)array[i] - 48;
    }
    return result;
}

char CelluloBluetooth::getHexChar(unsigned int value){
    if(value <= 0x9)
        return (char)(value + '0');
    else if(value <= 0xF)
        return (char)(value - 0xA + 'A');
    else
        return 'F';
}

