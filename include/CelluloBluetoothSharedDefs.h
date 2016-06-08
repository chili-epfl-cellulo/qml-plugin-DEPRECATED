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
 * @file CelluloBluetoothSharedDefs.h
 * @brief c (exclusive) header for definitions shared with the robot
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2016-06-07
 */

#ifndef CELLULOBLUETOOTHSHAREDDEFS_H
#define CELLULOBLUETOOTHSHAREDDEFS_H

/**
 * @brief Battery states
 */
#define BATTERY_STATE_ENUM_SHARED enum BatteryState { \
        BatteryStateDischarging = 0, /** No charger present, battery draining */ \
        BatteryStateLow = 1,         /** No charger present, battery low, will shut down */ \
        BatteryStateCharging = 2,    /** Charger present, battery charging */ \
        BatteryStateCharged = 3,     /** Charger present, battery full */ \
        BatteryStateShutdown = 4,    /** Charger charging disabled, voltage too low or battery not present */ \
        BatteryStateError = 5,       /** Thermal fault or charge timeout */ \
        BatteryStateNumElements \
}

/**
 * @brief Visual state types
 */
#define VISUAL_STATE_ENUM_SHARED enum VisualState { \
        VisualStateResponsive = 0, /** LEDs respond to touches by slightly increasing brightness */ \
        VisualStateAbsolute = 1,   /** LEDs don't respond to touches */ \
        VisualStateNumElements \
}

/**
 * @brief Visual effect types
 */
#define VISUAL_EFFECT_ENUM_SHARED enum VisualEffect { \
        VisualEffectConstAll = 0,    /** Set all LED colors (value unused) */ \
        VisualEffectConstSingle = 1, /** Set one LED color (value is LED index) */ \
        VisualEffectAlertAll = 2,    /** Alert animation for all LEDs (value unused) */ \
        VisualEffectAlertSingle = 3, /** Alert animation for one LED (value is LED index) */ \
        VisualEffectProgress = 4,    /** Static progress circularly (value 0-255 maps to 0-100%) */ \
        VisualEffectWaiting = 5,     /** Circular waiting/processing animation (value unused) */ \
        VisualEffectDirection = 6,   /** Point toward one direction (value 0-255 maps to 0-360 degrees) */ \
        VisualEffectBlink = 7,       /** Alert forever (value*20 is LED on time in milliseconds) */ \
        VisualEffectBreathe = 8,     /** Breathe animation (value unused) */ \
        VisualEffectPulse = 9,       /** Slower breathe-like animation (value unused) */ \
        VisualEffectNumElements \
}

/**
 * @brief App to robot message types
 */
#define COMMAND_PACKET_TYPE_ENUM_SHARED enum CmdPacketType { \
        CmdPacketTypePing = 0,            /** Ping robot, which will send an acknowledge event */ \
        CmdPacketTypeSetBcastPeriod,      /** Set pose broadcast period */ \
        CmdPacketTypeTimestampEnable,     /** Enable sending timestamp along with pose */ \
        CmdPacketTypeFrameRequest,        /** Request a camera image frame */ \
        CmdPacketTypeBatteryStateRequest, /** Request battery state */ \
        CmdPacketTypeSetVisualState,      /** Set visual state */ \
        CmdPacketTypeSetVisualEffect,     /** Set visual effect */ \
        CmdPacketTypeSetMotorOutput,      /** Set output of one motor */ \
        CmdPacketTypeSetAllMotorOutputs,  /** Set output of all motors */ \
        CmdPacketTypeSetGoalVelocity,     /** Set goal global velocity */ \
        DEPRECATED,                       /** DEPRECATED */ \
        CmdPacketTypeSetGoalPose,         /** Set goal pose */ \
        CmdPacketTypeSetGoalPosition,     /** Set goal position */ \
        CmdPacketTypeReset,               /** Request reset */ \
        CmdPacketTypeShutdown,            /** Request shutdown */ \
        CmdPacketTypeNumElements, \
}

/*
 * @brief App to robot message identifiers
 */
#define COMMAND_PACKET_STR_SHARED { \
        "P", /** CmdPacketTypePing */ \
        "D", /** CmdPacketTypeSetBcastPeriod */ \
        "T", /** CmdPacketTypeTimestampEnable */ \
        "F", /** CmdPacketTypeFrameRequest */ \
        "B", /** CmdPacketTypeBatteryStateRequest */ \
        "V", /** CmdPacketTypeSetVisualState */ \
        "E", /** CmdPacketTypeSetVisualEffect */ \
        "M", /** CmdPacketTypeSetMotorOutput */ \
        "A", /** CmdPacketTypeSetAllMotorOutputs */ \
        "C", /** CmdPacketTypeSetGoalVelocity */ \
        "Y", /** DEPRECATED */ \
        "G", /** CmdPacketTypeSetGoalPose */ \
        "N", /** CmdPacketTypeSetGoalPosition */ \
        "R", /** CmdPacketTypeReset */ \
        "S"  /** CmdPacketTypeShutdown */ \
}

/*
 * @brief App to robot message payload lengths
 */
#define COMMAND_PACKET_PAYLOAD_LEN_SHARED { \
        0,                 /** CmdPacketTypePing */ \
        2,                 /** CmdPacketTypeSetBcastPeriod: uint16 periodMillis */ \
        1,                 /** CmdPacketTypeTimestampEnable: uint8 bool */ \
        0,                 /** CmdPacketTypeFrameRequest */ \
        0,                 /** CmdPacketTypeBatteryStateRequest */ \
        1,                 /** CmdPacketTypeSetVisualState: uint8 state */ \
        1 + 1 + 1 + 1 + 1, /** CmdPacketTypeSetVisualEffect: uint8 effect, uint8 red, uint8 green, uint8 blue, uint8 value */ \
        1 + 2,             /** CmdPacketTypeSetMotorOutput: uint8 motorIndex, int16 output (-0xFFF to 0xFFF) */ \
        2 + 2 + 2,         /** CmdPacketTypeSetAllMotorOutputs: int16 m1Output, int16 m2Output, int16 m3Output (-0xFFF to 0xFFF) */ \
        2 + 2 + 2,         /** CmdPacketTypeSetGoalVelocity: int16 vx, int16 vy, int16 w */ \
        -1,                /** DEPRECATED */ \
        4 + 4 + 2 + 2 + 2, /** CmdPacketTypeSetGoalPose: uint32 x, uint32 y, uint16 theta, uint16 vMax, uint6 wMax */ \
        4 + 4 + 2,         /** CmdPacketTypeSetGoalPosition: uint32 x, uint32 y, uint16 vMax */ \
        0,                 /** CmdPacketTypeReset */ \
        0                  /** CmdPacketTypeShutdown */ \
}

/**
 * @brief Robot to app message types
 */
#define EVENT_PACKET_TYPE_ENUM_SHARED enum EventPacketType { \
        EventPacketTypeBootComplete = 0,       /** Boot completed after reset */ \
        EventPacketTypeShuttingDown,           /** About to shut down */ \
        EventPacketTypeLowBattery,             /** Battery is low, about to shut down */ \
        EventPacketTypeBatteryStateChanged,    /** Battery state is changed */ \
        EventPacketTypeTouchBegan,             /** Key was touched */ \
        EventPacketTypeTouchLongPressed,       /** Key was touched for a time */ \
        EventPacketTypeTouchReleased,          /** Key was released */ \
        EventPacketTypePoseChanged,            /** Pose changed */ \
        EventPacketTypePoseChangedTimestamped, /** Pose changed, timestamp attached to message */ \
        EventPacketTypeKidnapChanged,          /** Kidnap state changed */ \
        EventPacketTypeAcknowledged,           /** Acknowledged */ \
        EventPacketTypeFrameLine,              /** Camera frame line is sent */ \
        EventPacketTypeDebug,                  /** Debug message */ \
        EventPacketTypeNumElements \
}

/**
 * @brief Robot to app message identifiers
 */
#define EVENT_PACKET_STR_SHARED { \
        "O", /** EventPacketTypeBootComplete */ \
        "D", /** EventPacketTypeShuttingDown */ \
        "W", /** EventPacketTypeLowBattery */ \
        "B", /** EventPacketTypeBatteryStateChanged */ \
        "T", /** EventPacketTypeTouchBegan */ \
        "L", /** EventPacketTypeTouchLongPressed */ \
        "R", /** EventPacketTypeTouchReleased */ \
        "P", /** EventPacketTypePoseChanged */ \
        "S", /** EventPacketTypePoseChangedTimestamped */ \
        "K", /** EventPacketTypeKidnapChanged */ \
        "A", /** EventPacketTypeAcknowledged */ \
        "C", /** EventPacketTypeFrameLine */ \
        "E"  /** EventPacketTypeDebug */ \
}

/**
 * @brief Robot to app message payload lengths
 */
#define EVENT_PACKET_PAYLOAD_LEN_SHARED { \
        0,             /** EventPacketTypeBootComplete */ \
        0,             /** EventPacketTypeShuttingDown */ \
        0,             /** EventPacketTypeLowBattery */ \
        1,             /** EventPacketTypeBatteryStateChanged */ \
        1,             /** EventPacketTypeTouchBegan */ \
        1,             /** EventPacketTypeTouchLongPressed */ \
        1,             /** EventPacketTypeTouchReleased */ \
        4 + 4 + 2,     /** EventPacketTypePoseChanged */ \
        4 + 4 + 2 + 4, /** EventPacketTypePoseChangedTimestamped */ \
        1,             /** EventPacketTypeKidnapChanged */ \
        0,             /** EventPacketTypeAcknowledged */ \
        2 + IMG_WIDTH, /** EventPacketTypeFrameLine */ \
        0              /** EventPacketTypeDebug */ \
}

#endif // CELLULOBLUETOOTHSHAREDDEFS_H
