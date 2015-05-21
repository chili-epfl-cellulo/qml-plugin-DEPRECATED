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

const char* CelluloBluetooth::commandStrings[] = {
    "P", //(P)ing
    "F", //Request (f)rame
    "B", //Query (b)attery state
    "V", //Set (v)isual state
    "E", //Set (e)ffect
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

CelluloBluetooth::CelluloBluetooth(QQuickItem* parent) :
    QQuickItem(parent)
{
    socket = NULL;
    commandTimeout.setSingleShot(true);
    connect(&commandTimeout, SIGNAL(timeout()), this, SLOT(serverTimeout()));
}

CelluloBluetooth::~CelluloBluetooth(){ }

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
}

void CelluloBluetooth::socketDisconnected(){
    qDebug() << "Disconnected from " << macAddr << ", will try to reconnect";
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
    for(int i=0;i<message.length();i++)

        //End of transmission
        if(message[i] == '\n')
            processResponse();

        //Transmission continues
        else
            receiveBuffer += message[i];
}

void CelluloBluetooth::processResponse(){
    qDebug() << macAddr << " has sent: " << receiveBuffer;

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
                    emit batteryStateChanged(newState);

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

            //TODO

            break;

        case KIDNAP:

            //TODO

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

void CelluloBluetooth::sendCommand(){
    commandTimeout.stop();

    if(!commands.empty() && socket != NULL){
        qDebug() << "Sending command to " << macAddr << ": " << commands.head().message;
        socket->write(commands.head().message);

        commandTimeout.start(COMMAND_TIMEOUT_MILLIS);
    }
}

void CelluloBluetooth::ping(){
    QueuedCommand command;

    command.type = COMMAND_TYPE::PING;
    command.message = commandStrings[COMMAND_TYPE::PING];
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

void CelluloBluetooth::queryBatteryState(){
    QueuedCommand command;

    command.type = COMMAND_TYPE::BATTERY_STATE_REQUEST;
    command.message = commandStrings[COMMAND_TYPE::BATTERY_STATE_REQUEST];
    command.message.append('\n');

    commands.enqueue(command);

    if(commands.count() == 1)
        sendCommand();
}

