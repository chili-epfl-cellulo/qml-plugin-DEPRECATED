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
 * @file CelluloBluetoothEnums.h
 * @brief Header for CelluloBluetooth enums
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2016-06-07
 */

#ifndef CELLULOBLUETOOTHENUMS_H
#define CELLULOBLUETOOTHENUMS_H

#include<QObject>

#include"CelluloEnumDecl.h"

/**
 * @brief Wrapper containing all the enums of CelluloBluetooth
 */
class CelluloBluetoothEnums : public QObject {
/* *INDENT-OFF* */
Q_OBJECT
/* *INDENT-ON* */

public:

    CelluloBluetoothEnums(QObject* parent = 0);
    ~CelluloBluetoothEnums();

    /**
     * @brief Bluetooth connection status
     */
    enum ConnectionStatus {
        ConnectionStatusDisconnected = 0, ///< Idle and not connected
        ConnectionStatusConnecting,       ///< Actively trying to connect
        ConnectionStatusConnected,        ///< Connected
        ConnectionStatusNumElements
    };
    Q_ENUM(ConnectionStatus)
    CELLULO_ENUM_DECL(ConnectionStatus)

    /**
     * @brief Battery state of the robot
     */
    enum BatteryState {
        BatteryStateDischarging = 0,    ///< No charger present, battery draining
        BatteryStateLow = 1,            ///< No charger present, battery low, will shut down
        BatteryStateCharging = 2,       ///< Charger present, battery charging
        BatteryStateCharged = 3,        ///< Charger present, battery full
        BatteryStateShutdown = 4,       ///< Charger charging disabled, voltage too low or battery not present
        BatteryStateError = 5,          ///< Thermal fault or charge timeout
        BatteryStateNumElements
    };
    Q_ENUM(BatteryState)
    CELLULO_ENUM_DECL(BatteryState)

    /**
     * @brief Visual state of the robot
     */
    enum VisualState {
        VisualStateResponsive = 0,
        VisualStateAbsolute = 1,
        VisualStateNumElements
    };
    Q_ENUM(VisualState)
    CELLULO_ENUM_DECL(VisualState)

    /**
     * @brief List of possible visual effects
     */
    enum VisualEffect {
        VisualEffectConstAll = 0,    ///< Set all LED colors (value unused)
        VisualEffectConstSingle = 1, ///< Set one LED color (value is LED index)
        VisualEffectAlertAll = 2,    ///< Alert animation for all LEDs (value unused)
        VisualEffectAlertSingle = 3, ///< Alert animation for one LED (value is LED index)
        VisualEffectProgress = 4,    ///< Static progress circularly (value 0-255 maps to 0-100%)
        VisualEffectWaiting = 5,     ///< Circular waiting/processing animation (value unused)
        VisualEffectDirection = 6,   ///< Point toward one direction (value 0-255 maps to 0-360 degrees)
        VisualEffectBlink = 7,       ///< Alert forever (value*20 is LED on time in milliseconds)
        VisualEffectBreathe = 8,     ///< Breathe animation (value unused)
        VisualEffectPulse = 9,       ///< Slower breathe-like animation (value unused)
        VisualEffectNumElements
    };
    Q_ENUM(VisualEffect)
    CELLULO_ENUM_DECL(VisualEffect)
};

Q_DECLARE_METATYPE(CelluloBluetoothEnums::ConnectionStatus)
Q_DECLARE_METATYPE(CelluloBluetoothEnums::BatteryState)
Q_DECLARE_METATYPE(CelluloBluetoothEnums::VisualState)
Q_DECLARE_METATYPE(CelluloBluetoothEnums::VisualEffect)

#endif // CELLULOBLUETOOTHENUMS_H
