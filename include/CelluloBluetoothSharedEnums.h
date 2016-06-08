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
 * @file CelluloBluetoothSharedEnums.h
 * @brief c (exclusive) header with macros for enums shared with the robot
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2016-06-07
 */

#ifndef CELLULOBLUETOOTHSHAREDENUMS_H
#define CELLULOBLUETOOTHSHAREDENUMS_H

#define BATTERY_STATE_SHARED_ENUM enum BatteryState { \
        BatteryStateDischarging = 0, /** No charger present, battery draining */ \
        BatteryStateLow = 1,         /** No charger present, battery low, will shut down */ \
        BatteryStateCharging = 2,    /** Charger present, battery charging */ \
        BatteryStateCharged = 3,     /** Charger present, battery full */ \
        BatteryStateShutdown = 4,    /** Charger charging disabled, voltage too low or battery not present */ \
        BatteryStateError = 5,       /** Thermal fault or charge timeout */ \
        BatteryStateNumElements \
}

#define VISUAL_STATE_SHARED_ENUM enum VisualState { \
        VisualStateResponsive = 0, /** LEDs respond to touches by slightly increasing brightness */ \
        VisualStateAbsolute = 1,   /** LEDs don't respond to touches */ \
        VisualStateNumElements \
}

#define VISUAL_EFFECT_SHARED_ENUM enum VisualEffect { \
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

#endif // CELLULOBLUETOOTHSHAREENUMS_H
