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
 * @file CelluloBluetoothPacket.cpp
 * @brief Source for Bluetooth messages to/from Cellulo robots
 * @author Ayberk Özgür
 */

#include "CelluloBluetoothPacket.h"

#include <QMetaObject>
#include <QMetaEnum>

const char* CelluloBluetoothPacket::sendPacketTypeStr[] = {
    "P", //PING
    "D", //SET_BCAST_PERIOD
    "T", //TIMESTAMP_ENABLE
    "F", //FRAME_REQUEST
    "B", //BATTERY_STATE_REQUEST
    "V", //SET_VISUAL_STATE
    "E", //SET_VISUAL_EFFECT
    "M", //SET_MOTOR_OUTPUT
    "A", //SET_ALL_MOTOR_OUTPUTS
    "C", //SET_GOAL_VELOCITY
    "Y", //Set goal velocit(y) compact
    "G", //SET_GOAL_POSE
    "N", //SET_GOAL_POSITION
    "R", //RESET
    "S"  //SHUTDOWN
};

const int CelluloBluetoothPacket::sendPacketPayloadLen[] = {
    0,                 //PING
    2,                 //SET_BCAST_PERIOD
    1,                 //TIMESTAMP_ENABLE
    0,                 //FRAME_REQUEST
    0,                 //BATTERY_STATE_REQUEST
    1,                 //SET_VISUAL_STATE
    1 + 1 + 1 + 1 + 1, //SET_VISUAL_EFFECT
    1 + 2,             //SET_MOTOR_OUTPUT
    2 + 2 + 2,         //SET_ALL_MOTOR_OUTPUTS
    2 + 2 + 2,         //SET_GOAL_VELOCITY
    -1,                //Set goal velocity compact
    4 + 4 + 2 + 2 + 2, //SET_GOAL_POSE
    4 + 4 + 2,         //SET_GOAL_POSITION
    0,                 //RESET
    0                  //SHUTDOWN
};

const char* CelluloBluetoothPacket::receivePacketTypeStr[] = {
    "O", //BOOT_COMPLETE
    "H", //WAKE_UP
    "D", //SHUTTING_DOWN
    "W", //LOW_BATTERY
    "B", //BATTERY_STATE_CHANGED
    "T", //TOUCH_BEGIN
    "L", //TOUCH_LONG_PRESSED
    "R", //TOUCH_RELEASED
    "P", //POSE_CHANGED
    "S", //POSE_CHANGED_TIMESTAMPED
    "K", //KIDNAP
    "A", //ACKNOWLEDGED
    "C", //CAMERA_IMAGE_LINE
    "E"  //DEBUG
};

const int CelluloBluetoothPacket::receivePacketPayloadLen[] = {
    0,             //BOOT_COMPLETE
    0,             //WAKE_UP
    0,             //SHUTTING_DOWN
    0,             //LOW_BATTERY
    1,             //BATTERY_STATE_CHANGED
    1,             //TOUCH_BEGIN
    1,             //TOUCH_LONG_PRESSED
    1,             //TOUCH_RELEASED
    4 + 4 + 2,     //POSE_CHANGED
    4 + 4 + 2 + 4, //POSE_CHANGED_TIMESTAMPED
    1,             //KIDNAP
    0,             //ACKNOWLEDGED
    2 + IMG_WIDTH, //CAMERA_IMAGE_LINE
    0              //DEBUG
};

CelluloBluetoothPacket::operator QString() const {
    QString str;
    const QMetaObject& metaObject = CelluloBluetoothPacket::staticMetaObject;
    QMetaEnum metaEnum;

    metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("SEND_PACKET_TYPE"));
    str += "sendPacketType: ";
    str += metaEnum.valueToKey((int)sendPacketType);

    metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("RECEIVE_PACKET_TYPE"));
    str += "; receivePacketType: ";
    str += metaEnum.valueToKey((int)receivePacketType);

    metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("RECEIVE_STATUS"));
    str += "; receiveStatus: ";
    str += metaEnum.valueToKey((int)receiveStatus);

    str += "; payload:";
    QByteArray hex = payload.toHex().toUpper();
    for(int i=0; i<hex.length(); i+=2){
        str += " 0x";
        str += hex[i];
        str += hex[i + 1];
    }

    return str;
}

CelluloBluetoothPacket::CelluloBluetoothPacket(){
    clear();
}

CelluloBluetoothPacket::~CelluloBluetoothPacket(){ }

void CelluloBluetoothPacket::setSendPacketType(SEND_PACKET_TYPE type){
    sendPacketType = type;
}

void CelluloBluetoothPacket::clear(){
    sendPacketType = SEND_PACKET_TYPE::INVALID_PACKET_TYPE;
    receivePacketType = RECEIVE_PACKET_TYPE::INVALID_PACKET_TYPE;

    receiveStatus = RECEIVE_STATUS::NOT_RECEIVING;
    receiveBytesRemaining = -1;

    payload.clear();
    unloadIndex = 0;
}

void CelluloBluetoothPacket::load(quint32 num){

    //Convert to big endian
    char* p = ((char*)&num);
    payload.append(p[3]);
    payload.append(p[2]);
    payload.append(p[1]);
    payload.append(p[0]);
}

void CelluloBluetoothPacket::load24(quint32 num){
    if(num > 0xFFFFFF){
        num = 0xFFFFFF;
        qDebug() << "CelluloBluetoothPacket::load24(quint32 num): Number loaded too large, clamping to " << 0xFFFFFF;
    }

    //Convert to big endian
    char* p = ((char*)&num);
    payload.append(p[2]);
    payload.append(p[1]);
    payload.append(p[0]);
}

void CelluloBluetoothPacket::load(quint16 num){

    //Convert to big endian
    char* p = ((char*)&num);
    payload.append(p[1]);
    payload.append(p[0]);
}

void CelluloBluetoothPacket::load(quint8 num){
    payload.append(*((char*)&num));
}

void CelluloBluetoothPacket::load(qint32 num){

    //Convert to big endian
    char* p = ((char*)&num);
    payload.append(p[3]);
    payload.append(p[2]);
    payload.append(p[1]);
    payload.append(p[0]);
}

void CelluloBluetoothPacket::load24(qint32 num){
    if(num > 0x7FFFFF){
        num = 0x7FFFFF;
        qDebug() << "CelluloBluetoothPacket::load24(qint32 num): Number loaded too large, clamping to " << 0x7FFFFF;
    }
    else if(num < -0x800000){
        num = -0x800000;
        qDebug() << "CelluloBluetoothPacket::load24(qint32 num): Number loaded too small, clamping to " << -0x800000;
    }

    //Convert to big endian
    char* p = ((char*)&num);
    payload.append(p[2]);
    payload.append(p[1]);
    payload.append(p[0]);
}

void CelluloBluetoothPacket::load(qint16 num){

    //Convert to big endian
    char* p = ((char*)&num);
    payload.append(p[1]);
    payload.append(p[0]);
}

void CelluloBluetoothPacket::load(qint8 num){
    payload.append(*((char*)&num));
}

QByteArray CelluloBluetoothPacket::getSendData(){
    QByteArray data;

    data.append(PACKET_START_CHAR);
    data.append(sendPacketTypeStr[(int)sendPacketType]);
    data.append(payload);

    return data;
}

bool CelluloBluetoothPacket::loadReceivedByte(char c){
    switch(receiveStatus){
        case RECEIVE_STATUS::NOT_RECEIVING:
            if(c == PACKET_START_CHAR)
                receiveStatus = RECEIVE_STATUS::WAITING_FOR_TYPE;
            return false;

        case RECEIVE_STATUS::WAITING_FOR_TYPE:

            //Determine type
            receivePacketType = RECEIVE_PACKET_TYPE::INVALID_PACKET_TYPE;
            for(int i=0; i<(int)RECEIVE_PACKET_TYPE::NUM_RECEIVE_PACKET_TYPES; i++)
                if(receivePacketTypeStr[i][0] == c){
                    receivePacketType = (RECEIVE_PACKET_TYPE)i;
                    break;
                }

            //Valid packet type
            if(receivePacketType != RECEIVE_PACKET_TYPE::INVALID_PACKET_TYPE){
                receiveBytesRemaining = receivePacketPayloadLen[(int)receivePacketType];
                if(receiveBytesRemaining <= 0){
                    receiveStatus = RECEIVE_STATUS::END_OF_PACKET;
                    return true;
                }
                else{
                    receiveStatus = RECEIVE_STATUS::PAYLOAD_RECEIVING;
                    return false;
                }
            }

            //Invalid packet type, reset packet
            else{
                clear();
                return false;
            }

        case RECEIVE_STATUS::PAYLOAD_RECEIVING:
            payload.append(c);
            receiveBytesRemaining--;
            if(receiveBytesRemaining <= 0){
                receiveStatus = RECEIVE_STATUS::END_OF_PACKET;
                return true;
            }
            else
                return false;

        case RECEIVE_STATUS::END_OF_PACKET:
            if(c == PACKET_START_CHAR){
                clear();
                receiveStatus = RECEIVE_STATUS::WAITING_FOR_TYPE;
            }
            return false;

        default:
            return false;
    }
}

quint32 CelluloBluetoothPacket::unloadUInt32(){
    if(unloadIndex + 4 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadUInt32(): Unload index out of bounds";
        return 0;
    }
    else{

        //Decode from big endian
        unsigned char* p = (unsigned char*)payload.data() + unloadIndex;
        unloadIndex += 4;
        return (quint32)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3] << 0));
    }
}

quint32 CelluloBluetoothPacket::unloadUInt24(){
    if(unloadIndex + 3 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadUInt24(): Unload index out of bounds";
        return 0;
    }
    else{

        //Decode from big endian
        unsigned char* p = (unsigned char*)payload.data() + unloadIndex;
        unloadIndex += 3;
        return (quint32)((p[0] << 16) | (p[1] << 8) | (p[2] << 0));
    }
}

quint16 CelluloBluetoothPacket::unloadUInt16(){
    if(unloadIndex + 2 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadUInt16(): Unload index out of bounds";
        return 0;
    }
    else{

        //Decode from big endian
        unsigned char* p = (unsigned char*)payload.data() + unloadIndex;
        unloadIndex += 2;
        return (quint16)((p[0] << 8) | (p[1] << 0));
    }
}

quint8 CelluloBluetoothPacket::unloadUInt8(){
    if(unloadIndex + 1 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadUInt8(): Unload index out of bounds";
        return 0;
    }
    else{
        quint8 result = (quint8)(*((unsigned char*)payload.data() + unloadIndex));
        unloadIndex++;
        return result;
    }
}

qint32 CelluloBluetoothPacket::unloadInt32(){
    if(unloadIndex + 4 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadInt32(): Unload index out of bounds";
        return 0;
    }
    else{

        //Decode from big endian
        unsigned char* p = (unsigned char*)payload.data() + unloadIndex;
        unloadIndex += 4;
        quint32 unum = (quint32)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3] << 0));
        return *((qint32*)(&unum));
    }
}

qint32 CelluloBluetoothPacket::unloadInt24(){
    if(unloadIndex + 3 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadInt24(): Unload index out of bounds";
        return 0;
    }
    else{

        //Decode from big endian
        unsigned char* p = (unsigned char*)payload.data() + unloadIndex;
        unloadIndex += 3;
        quint32 unum = (quint32)((p[0] << 16) | (p[1] << 8) | (p[2] << 0));

        //Sign extension
        if(unum >= 0x800000)
            unum += 0xFF000000;
        return *((qint32*)(&unum));
    }
}

qint16 CelluloBluetoothPacket::unloadInt16(){
    if(unloadIndex + 2 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadInt16(): Unload index out of bounds";
        return 0;
    }
    else{

        //Decode from big endian
        unsigned char* p = (unsigned char*)payload.data() + unloadIndex;
        unloadIndex += 2;
        quint16 unum = (quint16)((p[0] << 8) | (p[1] << 0));
        return *((qint16*)(&unum));
    }
}

qint8 CelluloBluetoothPacket::unloadInt8(){
    if(unloadIndex + 1 > payload.length()){
        qDebug() << "CelluloBluetoothPacket::unloadInt8(): Unload index out of bounds";
        return 0;
    }
    else{
        qint8 result = (qint8)(*((char*)payload.data() + unloadIndex));
        unloadIndex++;
        return result;
    }
}
