/*
 * Copyright (C) 2016 EPFL
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
 * @file CelluloBluetoothPacket.h
 * @brief Header for a Bluetooth message to/from Cellulo robots
 * @author Ayberk Özgür
 */

#ifndef CELLULOBLUETOOTHPACKET_H
#define CELLULOBLUETOOTHPACKET_H

#include <QObject>
#include <QDebug>
#include <QtGlobal>
#include <QByteArray>

#include "CelluloBluetoothSharedDefs.h"

/**
 * @brief Bluetooth communicator for a Cellulo robot
 */
class CelluloBluetoothPacket : public QObject {
/* *INDENT-OFF* */
Q_OBJECT
/* *INDENT-ON* */

public:
    COMMAND_PACKET_TYPE_ENUM_SHARED;
    EVENT_PACKET_TYPE_ENUM_SHARED;

    /**
     * @brief Describes the status of the packet while it's being received
     */
    enum class RECEIVE_STATUS {
        NOT_RECEIVING = 0,
        WAITING_FOR_TYPE,
        PAYLOAD_RECEIVING,
        END_OF_PACKET,
        NUM_RECEIVE_STATUS
    };

    static const char* sendPacketTypeStr[];                       ///< Strings sent over Bluetooth to give commands
    static const char* receivePacketTypeStr[];                    ///< Strings received over Bluetooth as response or event

    static const int sendPacketPayloadLen[];                      ///< Total lengths of packets sent over Bluetooth
    static const int receivePacketPayloadLen[];                   ///< Total lengths of packets received over Bluetooth

    /**
     * @brief Creates a new Cellulo Bluetooth message
     */
    CelluloBluetoothPacket();

    /**
     * @brief Destroys this Cellulo Bluetooth message
     */
    virtual ~CelluloBluetoothPacket();

    /**
     * @brief Prettyprinting
     *
     * @return String representation of this object
     */
    operator QString() const;

    /**
     * @brief Sets the outgoing message type
     *
     * @param type Outgoing message type
     */
    void setSendPacketType(CmdPacketType type);

    /**
     * @brief Clears the payload
     */
    void clear();

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void load(quint32 num);

    /**
     * @brief Appends the first 24 bits of the given number in binary form (2's complement) to the payload
     *
     * Clamps the value to 24-bits if outside of bounds.
     *
     * @param num The number to append
     */
    void load24(quint32 num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void load(quint16 num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void load(quint8 num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void load(qint32 num);

    /**
     * @brief Appends the first 24 bits of the given number in binary form (2's complement) to the payload
     *
     * Clamps the value to 24-bits if outside of bounds.
     *
     * @param num The number to append
     */
    void load24(qint32 num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void load(qint16 num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void load(qint8 num);

    /**
     * @brief Constructs the message to be sent
     *
     * @return Message to be sent
     */
    QByteArray getSendData();

    /**
     * @brief Processes incoming byte, determines type if byte is relevant
     *
     * @param c Incoming byte
     * @return Whether the packet is completely received
     */
    bool loadReceivedByte(char c);

    /**
     * @brief Gets the received packet type if any
     *
     * @return The received packet type
     */
    EventPacketType getReceivePacketType(){
        return receivePacketType;
    }

    /**
     * @brief Attempts to unload a 32-bit unsigned integer
     *
     * @return Unloaded value if successful
     */
    quint32 unloadUInt32();

    /**
     * @brief Attempts to unload a 24-bit unsigned integer
     *
     * @return Unloaded value if successful
     */
    quint32 unloadUInt24();

    /**
     * @brief Attempts to unload a 16-bit unsigned integer
     *
     * @return Unloaded value if successful
     */
    quint16 unloadUInt16();

    /**
     * @brief Attempts to unload an 8-bit unsigned integer
     *
     * @return Unloaded value if successful
     */
    quint8 unloadUInt8();

    /**
     * @brief Attempts to unload a 32-bit signed integer
     *
     * @return Unloaded value if successful
     */
    qint32 unloadInt32();

    /**
     * @brief Attempts to unload a 24-bit signed integer
     *
     * @return Unloaded value if successful
     */
    qint32 unloadInt24();

    /**
     * @brief Attempts to unload a 16-bit signed integer
     *
     * @return Unloaded value if successful
     */
    qint16 unloadInt16();

    /**
     * @brief Attempts to unload a 8-bit signed integer
     *
     * @return Unloaded value if successful
     */
    qint8 unloadInt8();

private:

    CmdPacketType sendPacketType;           ///< Packet type if outgoing packet
    EventPacketType receivePacketType;      ///< Packet type if incoming packet

    RECEIVE_STATUS receiveStatus;           ///< Current status if receiving packet
    int receiveBytesRemaining;              ///< Number of bytes left to receive

    QByteArray payload;                     ///< Payload of the actual packet
    int unloadIndex;                        ///< Current index to unload from the payload
};

#endif // CELLULOBLUETOOTHPACKET_H
