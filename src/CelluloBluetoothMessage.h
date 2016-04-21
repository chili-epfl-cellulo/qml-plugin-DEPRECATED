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
 * @file CelluloBluetoothMessage.h
 * @brief Header for a Bluetooth message to/from Cellulo robots
 * @author Ayberk Özgür
 */

#ifndef CELLULOBLUETOOTHMESSAGE_H
#define CELLULOBLUETOOTHMESSAGE_H

#include<cstdint>

#include<QByteArray>

/**
 * @brief Bluetooth communicator for a Cellulo robot
 */
class CelluloBluetoothMessage{

public:

    enum class SEND_MESSAGE_TYPE{
        PING = 0,
        SET_BCAST_PERIOD,
        IMAGE_STREAM_ENABLE,
        TIMESTAMP_ENABLE,
        FRAME_REQUEST,
        BATTERY_STATE_REQUEST,
        SET_VISUAL_STATE,
        SET_VISUAL_EFFECT,
        SET_MOTOR_OUTPUT,
        SET_ALL_MOTOR_OUTPUTS,
        SET_GOAL_VELOCITY,
        SET_GOAL_VELOCITY_COMPACT,
        SET_GOAL_POSE,
        SET_GOAL_POSITION,
        RESET,
        SHUTDOWN,
        NUM_SEND_MESSAGE_TYPES,
        INVALID_MESSAGE_TYPE = -1
    };

    enum class RECEIVE_MESSAGE_TYPE{
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
        DEBUG,
        NUM_RECEIVE_MESSAGE_TYPES,
        INVALID_MESSAGE_TYPE = -1
    };

    const char PACKET_START_CHAR = 0x01;

    static const int IMG_WIDTH = 752/4;              ///< Image width of the robot's camera
    static const int IMG_HEIGHT = 480/4;             ///< Image height of the robot's camera

    static constexpr float GOAL_POSE_FACTOR = 100.0f;       ///< Goal pose elements are multiplied by this before comm.
    static constexpr float GOAL_VELOCITY_FACTOR = 100.0f;   ///< Goal velocities are multiplied by this before comm.
    static const int GOAL_VELOCITY_COMPACT_DIVISOR = 2;     ///< Goal velocities are divided by this in the compact velocity command

    /**
     * @brief Creates a new Cellulo Bluetooth message
     */
    CelluloBluetoothMessage();

    /**
     * @brief Destroys this Cellulo robot communicator
     */
    virtual ~CelluloBluetoothMessage();

    /**
     * @brief Sets the outgoing message type
     *
     * @param type Outgoing message type
     */
    void setSendMessageType(SEND_MESSAGE_TYPE type);

    /**
     * @brief Clears the payload
     */
    void clearPayload();

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void loadNumber(uint32_t num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void loadNumber(uint16_t num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void loadNumber(uint8_t num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void loadNumber(int32_t num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void loadNumber(int16_t num);

    /**
     * @brief Appends the given number in binary form to the payload
     *
     * @param num The number to append
     */
    void loadNumber(int8_t num);

    /**
     * @brief Constructs the message to be sent
     *
     * @return Message to be sent
     */
    QByteArray getSendMessage();

    /**
     * @brief 
     *
     * @param receiveMessage
     */
    void parseReceiveMessage(QByteArray& receiveMessage);

private:

    static const char* sendMessageTypeStr[];    ///< Strings sent over Bluetooth to give commands
    static const char* receiveMessageTypeStr[]; ///< Strings received over Bluetooth as response or event

    static const int sendMessageTypeLen[];      ///< Total lengths of messages sent over Bluetooth
    static const int receiveMessageTypeLen[];   ///< Total lengths of messages received over Bluetooth

    SEND_MESSAGE_TYPE sendMessageType;          ///< Message type if outgoing message
    RECEIVE_MESSAGE_TYPE receiveMessageType;    ///< Message type if incoming message
    QByteArray payload;                         ///< Payload of the actual message

    /**
     * @brief Sends command over Bluetooth
     *
     * @param type Type of the command
     * @param message Message body itself
     */
    void sendCommand(COMMAND_TYPE type, QByteArray& message);

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

    /**
     * @brief Calculates the integer value from an uppercase hex string
     *
     * @param array Array containing the hex string
     * @param begin Index of most significant digit
     * @param end Index of least significant digit
     *
     * @return The value
     */
    int hexToInt(QByteArray const& array, int begin, int end);

    /**
     * @brief Converts single digit integer to hexadecimal character
     *
     * @param value Between 0x0 and 0xF
     *
     * @return Hexadecimal representation of value
     */
    char getHexChar(unsigned int value);

    /**
     * @brief Calculates the number of 1 bits in the given value
     *
     * @param value Given value
     *
     * @return Number of 1 bits in given value
     */
    char getNumberOfOnes(unsigned int value);
};

#endif // CELLULOBLUETOOTH_H
