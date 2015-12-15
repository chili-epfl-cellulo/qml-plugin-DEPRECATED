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
    "F", //Request (f)rame
    "B", //Query (b)attery state
    "V", //Set (v)isual state
    "E", //Set (e)ffect
    "M", //Set (m)otor output
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
    "N"  //(N)ot acknowledged
};

QByteArray CelluloBluetooth::frameBuffer;

CelluloBluetooth::CelluloBluetooth(QQuickItem* parent) :
    QQuickItem(parent)
{
    socket = NULL;

    commandTimeout.setSingleShot(true);
    connect(&commandTimeout, SIGNAL(timeout()), this, SLOT(serverTimeout()));
    frameTimeoutTimer.setSingleShot(true);
    connect(&frameTimeoutTimer, SIGNAL(timeout()), this, SLOT(frameTimeout()));

    expectingFrame = false;
    frameBuffer.reserve(IMG_WIDTH*IMG_HEIGHT);

    frameLineEndSequence.append((char)0);
    frameLineEndSequence.append((char)255);
    frameLineEndSequence.append((char)0);
    frameLineEndSequence.append((char)255);

    connected = false;
    batteryState = 4; //Beginning with shutdown is a good idea
    x = 0;
    y = 0;
    theta = 0;
    kidnapped = true;
    profiling = false;
}

CelluloBluetooth::~CelluloBluetooth(){ }

QVariantList CelluloBluetooth::getFrame() const {
    QVariantList frame;
    for(int i=0;i<frameBuffer.length();i++)
        frame.append((int)frameBuffer[i]);
    while(frame.length() < IMG_WIDTH*IMG_HEIGHT)
        frame.append((int)0);
    return frame;
}

void CelluloBluetooth::setMacAddr(QString macAddr){
    this->macAddr = macAddr;
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    connect(socket, SIGNAL(readyRead()), this, SLOT(socketDataArrived()));
    connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

    reconnectToServer();
}

void CelluloBluetooth::socketConnected(){
    qDebug() << "Connected to " << macAddr;
    if(!commands.empty())
        sendCommand();
    if(!connected){
        connected = true;
        emit connectedChanged();
    }
}

void CelluloBluetooth::socketDisconnected(){
    qDebug() << "Disconnected from " << macAddr << ", will try to reconnect";
    if(connected){
        connected = false;
        emit connectedChanged();
    }
    reconnectToServer();
}

void CelluloBluetooth::reconnectToServer(){
    if(socket != NULL){
        qDebug() << "Connecting to " << macAddr << "...";

        socket->connectToService(
                QBluetoothAddress(macAddr),
                QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB"))); //Connect to the Serial Protocol Profile
    }
}

void CelluloBluetooth::socketDataArrived(){
    QByteArray message = socket->readAll();

    for(int i=0;i<message.length();i++){
        if(expectingFrame){
            receiveBuffer.append(message[i]);

            //If line is received correctly, or too many bytes are received without the end sequence
            if(receiveBuffer.length() >= IMG_WIDTH + frameLineEndSequence.length()){
                frameBuffer.append(receiveBuffer.left(IMG_WIDTH));
            }

            //If line end is received prematurely
            else if(receiveBuffer.endsWith(frameLineEndSequence)){
                receiveBuffer.chop(frameLineEndSequence.length());
                while(receiveBuffer.length() < IMG_WIDTH)
                    receiveBuffer.append((char)0);
                frameBuffer.append(receiveBuffer);
            }

            //If line is still being received
            else
                continue;

            qDebug() << macAddr << " sent line number " << currentLine;
            receiveBuffer.clear();
            currentLine++;
            if(currentLine >= IMG_HEIGHT){
                expectingFrame = false;
                frameTimeoutTimer.stop();
                qDebug() << macAddr << " sent complete frame";
                emit frameReady();
            }
        }
        else{
            //End of transmission
            if(message[i] == '\n')
                processResponse();

            //Transmission continues
            else
                receiveBuffer += message[i];
        }
    }
}

void CelluloBluetooth::processResponse(){
    //qDebug() << macAddr << " has sent: " << receiveBuffer;

    switch(getReceivedMessage()){
        case BOOT_COMPLETE:

            //Correct message length
            if(receiveBuffer.length() == 1)
                emit bootCompleted();
            break;

        case WAKE_UP:

            //Correct message length
            if(receiveBuffer.length() == 1)
                emit wokeUp();
            break;

        case SHUTTING_DOWN:

            //Correct message length
            if(receiveBuffer.length() == 1)
                emit shuttingDown();
            break;

        case LOW_BATTERY:

            //Correct message length
            if(receiveBuffer.length() == 1)
                emit lowBattery();
            break;

        case BATTERY_STATE_CHANGED:

            //Correct message length
            if(receiveBuffer.length() == 2){

                int newState = (int)(receiveBuffer[1] - 48);
                if(newState >= 0 && newState <= 4){
                    if(batteryState != newState){
                        batteryState = newState;
                        emit batteryStateChanged();
                    }

                    //If we received this as a reply to the command we sent, it is complete
                    if(!commands.empty() && commands.head().type == COMMAND_TYPE::BATTERY_STATE_REQUEST){
                        commands.dequeue();
                        sendCommand();
                    }
                }
                else{

                    //If we received this as a reply to the command we sent, resend to get a proper response
                    if(!commands.empty() && commands.head().type == COMMAND_TYPE::BATTERY_STATE_REQUEST)
                        sendCommand();

                    //If this was a spontaneous event and we got a corrupted response, ask explicitly
                    else
                        queryBatteryState();
                }
            }
            else{

                //If we received this as a reply to the command we sent, resend to get a proper response
                if(!commands.empty() && commands.head().type == COMMAND_TYPE::BATTERY_STATE_REQUEST)
                    sendCommand();

                //If this was a spontaneous event and we got a corrupted response, ask explicitly
                else
                    queryBatteryState();
            }
            break;

        case TOUCH_BEGIN:

            //Correct message length
            if(receiveBuffer.length() == 2){

                int key = (int)(receiveBuffer[1] - 48);
                if(key >= 0 && key <= 5)
                    emit touchBegan(key);
            }
            break;

        case TOUCH_LONG_PRESSED:

            //Correct message length
            if(receiveBuffer.length() == 2){

                int key = (int)(receiveBuffer[1] - 48);
                if(key >= 0 && key <= 5)
                    emit longTouch(key);
            }
            break;

        case TOUCH_RELEASED:

            //Correct message length
            if(receiveBuffer.length() == 2){

                int key = (int)(receiveBuffer[1] - 48);
                if(key >= 0 && key <= 5)
                    emit touchReleased(key);
            }
            break;

        case POSE_CHANGED:

            //Correct message length
            if(receiveBuffer.length() == 22){
                int receivedParity = (int)receiveBuffer[21] >= 65 ? (int)receiveBuffer[21] - 65 + 10 : (int)receiveBuffer[21] - 48;

                int x = hexToInt(receiveBuffer, 1, 8);
                int y = hexToInt(receiveBuffer, 9, 16);
                int theta = hexToInt(receiveBuffer, 17, 20);

                if(receivedParity != calculateChecksum(x,y,theta))
                    qDebug() << "PARITY CHECK FAILED";
                else{
                    this->x = x/100.0f;
                    this->y = y/100.0f;
                    this->theta = theta/100.0f;
                    emit poseChanged();
                    if(kidnapped){
                        kidnapped = false;
                        emit kidnappedChanged();
                    }
                }

                if (profiling) {
                    int timeElapsed = int(time(0))-timeStart;
                    poseChangeCount++;
                    decodingRate = poseChangeCount/(float)timeElapsed;
                    emit decodingRateChanged();
                }
            }
            break;

        case KIDNAP:

            //Correct message length
            if(receiveBuffer.length() == 2){
                int newKidnapped = (int)(receiveBuffer[1] - 48);
                if(newKidnapped == 0 || newKidnapped == 1)
                    if((bool)newKidnapped != kidnapped)
                        kidnapped = newKidnapped;
                        emit kidnappedChanged();
                    }
            break;

        case ACKNOWLEDGED:

            //Correct message length
            if(receiveBuffer.length() == 1){

                //This command transaction is complete, send next command if exists
                if(!commands.empty())
                    commands.dequeue();
                sendCommand();
            }

            //Incorrect message length, take as nack and send last message again
            else
                sendCommand();

            break;

        case NOT_ACKNOWLEDGED:

            //Probably our message did not reach properly, send last command if exists
            sendCommand();

            break;

        default:

            //Invalid message might be corrupted ack; send last command if exists
            sendCommand();
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

void CelluloBluetooth::serverTimeout(){
    qDebug() << macAddr << " timed out in responding to command";
    sendCommand();
}

void CelluloBluetooth::frameTimeout(){
    qDebug() << macAddr << " timed out in sending the camera frame";
    expectingFrame = false;
    emit frameReady();
}

void CelluloBluetooth::sendCommand(){
    commandTimeout.stop();

    if(!commands.empty() && socket != NULL){
        //qDebug() << "Sending command to " << macAddr << ": " << commands.head().message;

        if(commands.head().type == COMMAND_TYPE::FRAME_REQUEST){
            frameTimeoutTimer.start(FRAME_TIMEOUT_MILLIS);
            expectingFrame = true;
            currentLine = 0;
            frameBuffer.clear();
        }
        else
            commandTimeout.start(COMMAND_TIMEOUT_MILLIS);

        socket->write(commands.head().message);

        if(expectingFrame && !commands.empty()) //Empty check is just for safety
            commands.dequeue();
    }
}

void CelluloBluetooth::ping(){
    if(expectingFrame)
        return;

    QueuedCommand command;

    command.type = COMMAND_TYPE::PING;
    command.message = commandStrings[COMMAND_TYPE::PING];
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::requestFrame(){
    if(expectingFrame)
        return;

    QueuedCommand command;

    command.type = COMMAND_TYPE::FRAME_REQUEST;
    command.message = commandStrings[COMMAND_TYPE::FRAME_REQUEST];
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::queryBatteryState(){
    if(expectingFrame)
        return;

    QueuedCommand command;

    command.type = COMMAND_TYPE::BATTERY_STATE_REQUEST;
    command.message = commandStrings[COMMAND_TYPE::BATTERY_STATE_REQUEST];
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
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
    if(expectingFrame)
        return;

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

    QueuedCommand command;

    command.type = COMMAND_TYPE::SET_MOTOR_OUTPUT;
    command.message = commandStrings[COMMAND_TYPE::SET_MOTOR_OUTPUT];
    command.message.append((char)(motor + '0'));
    if(negative)
        command.message.append('-');
    command.message.append(getHexChar(output/0x100));
    command.message.append(getHexChar(output/0x10%0x10));
    command.message.append(getHexChar(output%0x10));
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::setGoalPose(float x, float y, float theta){
    if(expectingFrame)
        return;

    QueuedCommand command;

    command.type = COMMAND_TYPE::SET_GOAL_POSE;
    command.message = commandStrings[COMMAND_TYPE::SET_GOAL_POSE];
    static char buf[23 + 1];
    sprintf(buf,"%08X%08X%04X%X\n", (unsigned int)(100*x), (unsigned int)(100*y), (unsigned int)(100*theta),
            calculateChecksum((unsigned int)(100*x), (unsigned int)(100*y), (unsigned int)(100*theta)));
    command.message.append(buf);

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::setVisualState(int state){
    if(expectingFrame)
        return;

    QueuedCommand command;

    command.type = COMMAND_TYPE::SET_VISUAL_STATE;
    command.message = commandStrings[COMMAND_TYPE::SET_VISUAL_STATE];
    command.message.append((char)(state + 48));
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::setVisualEffect(int effect, QColor color, int value){
    if(expectingFrame)
        return;

    if(value > 255)
        value = 255;
    else if(value < 0)
        value = 0;

    QueuedCommand command;

    QString colorName = color.name(QColor::NameFormat::HexArgb); //In the form '#aarrggbb'
    colorName = colorName.toUpper();
    colorName.remove(0,3); //Remove # and alpha
    QString valueString;
    valueString.append((char)(value/16 >= 10 ? value/16 + 65 - 10 : value/16 + 48));
    valueString.append((char)(value%16 >= 10 ? value%16 + 65 - 10 : value%16 + 48));

    command.type = COMMAND_TYPE::SET_VISUAL_EFFECT;
    command.message = commandStrings[COMMAND_TYPE::SET_VISUAL_EFFECT];
    command.message.append((char)(effect + 48));
    command.message.append(colorName);
    command.message.append(valueString);
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::reset(){
    if(expectingFrame)
        return;

    QueuedCommand command;

    command.type = COMMAND_TYPE::RESET;
    command.message = commandStrings[COMMAND_TYPE::RESET];
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::toggleProfiling(){
    profiling = !profiling;
    poseChangeCount = 0;
    if (profiling)
        timeStart = time(0);
    emit profilingChanged();
}

void CelluloBluetooth::shutdown(){
    if(expectingFrame)
        return;

    QueuedCommand command;

    command.type = COMMAND_TYPE::SHUTDOWN;
    command.message = commandStrings[COMMAND_TYPE::SHUTDOWN];
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

int CelluloBluetooth::calculateChecksum(int x, int y, int theta){
    unsigned int i;
    unsigned int result = 0;
    for(i=0;i<32;i++){
        if(x % 2)
            result++;
        x >>= 1;

        if(y % 2)
            result++;
        y >>= 1;

        if(theta % 2)
            result++;
        theta >>= 1;
    }
    return result % 16;
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

