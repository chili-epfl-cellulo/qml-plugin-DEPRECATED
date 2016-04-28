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
    "Y", //Set goal velocit(y) compact
    "G", //Set (g)oal pose
    "N", //Set goal positio(n)
    "R", //(R)eset
    "S"  //(S)hutdown
};

const int CelluloBluetoothPacket::sendPacketPayloadLen[] = {
    0, //Ping
    -1, //Set broadcast period
    -1, //Enable image streaming + disable localization or vice-versa
    -1, //Enable timestamping along with pose and disable idling or vice-versa
    -1, //Request frame
    -1, //Query battery state
    -1, //Set visual state
    -1, //Set effect
    -1, //Set motor output
    -1, //Set all motor outputs
    -1, //Set goal velocity
    -1, //Set goal velocity compact
    -1, //Set goal pose
    -1, //Set goal position
    0, //Reset
    0  //Shutdown
};

const char* CelluloBluetoothPacket::receivePacketTypeStr[] = {
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

const int CelluloBluetoothPacket::receivePacketPayloadLen[] = {
    0, //Boot finished
    0, //Wake up finished (Hello)
    0, //Shutting down
    0, //Battery low, shutting down
    -1, //Battery state changed
    -1, //Touch began
    -1, //Long touch
    -1, //Touch released
    -1, //Pose changed
    -1, //Robot kidnapped or unkidnapped
    0, //Acknowledged
    2  //Debug message
};

CelluloBluetoothPacket::operator QString() const{
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
    for(int i=0;i<hex.length();i+=2){
        str += " 0x";
        str += hex[i];
        str += hex[i + 1];
    }

    return str;
}

CelluloBluetoothPacket::CelluloBluetoothPacket(){
    clear();
}

CelluloBluetoothPacket::~CelluloBluetoothPacket(){
}

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

    //Assume little endian storage and convert to big endian
    char* p = ((char*)&num) + 3;
    payload.append(*p);
    p--;
    payload.append(*p);
    p--;
    payload.append(*p);
    p--;
    payload.append(*p);
}

void CelluloBluetoothPacket::load(quint16 num){

    //Assume little endian storage and convert to big endian
    char* p = ((char*)&num) + 1;
    payload.append(*p);
    p--;
    payload.append(*p);
}

void CelluloBluetoothPacket::load(quint8 num){
    payload.append(*((char*)&num));
}

void CelluloBluetoothPacket::load(qint32 num){

    //Assume little endian storage and convert to big endian
    char* p = ((char*)&num) + 3;
    payload.append(*p);
    p--;
    payload.append(*p);
    p--;
    payload.append(*p);
    p--;
    payload.append(*p);
}

void CelluloBluetoothPacket::load(qint16 num){

    //Assume little endian storage and convert to big endian
    char* p = ((char*)&num) + 1;
    payload.append(*p);
    p--;
    payload.append(*p);
}

void CelluloBluetoothPacket::load(qint8 num){
    payload.append(*((char*)&num));
}

QByteArray CelluloBluetoothPacket::getSendPacket(){
    QByteArray packet;

    packet.append(PACKET_START_CHAR);
    packet.append(sendPacketTypeStr[(int)sendPacketType]);
    packet.append(payload);

    return packet;
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
            for(int i=0;i<(int)RECEIVE_PACKET_TYPE::NUM_RECEIVE_PACKET_TYPES;i++)
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
    //unsigned char* p = (unsigned char*)payload.data() + unloadIndex;

}

quint16 CelluloBluetoothPacket::unloadUInt16(){

}

quint8 CelluloBluetoothPacket::unloadUInt8(){

}

qint32 CelluloBluetoothPacket::unloadInt32(){

}

qint16 CelluloBluetoothPacket::unloadInt16(){

}

qint8 CelluloBluetoothPacket::unloadInt8(){

}
