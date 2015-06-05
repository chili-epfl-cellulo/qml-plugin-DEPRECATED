import QtQuick 2.0

import Qt.WebSockets 1.0

import CelluloBluetooth 1.0

import "cellulo-simulator-toolkit.js" as CelluloToolkit

Rectangle {
    id: robot
    property int robotId: 0
    width: 70
    height: 70
    color: "white"
    radius: 35
    border.width: 2
    rotation: 0

    // the physical diameter of the robot
    property real physicalSize: 5.5

    property real scalingFactor: 1
    property var playground: ({})

    property bool effectRunning: false

    property string robotMacAddress


    // Real robot
    CelluloBluetooth{
        id: robotComm
        macAddr: robotMacAddress

        //Signals
        onBootCompleted:        console.log("Robot " + robotId + " completed its (re)boot")
        onWokeUp:               console.log("Robot " + robotId + " completed its wake-up")
        onShuttingDown:         console.log("Robot " + robotId + " going to sleep (user request)")
        onLowBattery:           console.log("Robot " + robotId + " going to sleep (low battery)")
        onBatteryStateChanged:{
            console.log("Robot " + robotId + " battery state changed to: " + batteryState);
            parent.batteryState = batteryState;
        }
        onTouchBegan:{
            console.log("Robot " + robotId + " key " + key + "was touched");
            addTouch(key);
        }
        onLongTouch:            console.log("Robot " + robotId + " key " + key + "was long touched")
        onTouchReleased:        console.log("Robot " + robotId + " key " + key + "was released")
        onPoseChanged:{
            var pixelsCoords = CelluloToolkit.grid2mm(x*100, y*100, playground);
            //coords.x = gridCoords.x/playground.width;
            //coords.y = gridCoords.y/playground.height;

            robot.x = pixelsCoords.x
            robot.y = pixelsCoords.y
            robot.rotation = -theta; // degrees
            if(x > 0 && y > 0) {
                console.log("Robot %1 at (%2, %3), theta: %4".arg(robotId).arg(robot.x).arg(robot.y).arg(rotation));
            }
        }
        onKidnappedChanged:     kidnapped ? console.log("Robot " + robotId + " was kidnapped") : console.log("Robot " + robotId + " was returned on paper")
    }


    Pad {
        id: pad0
        padId:0
    }
    Pad {
        id: pad1
        padId:1
    }
    Pad {
        id: pad2
        padId:2
    }
    Pad {
        id: pad3
        padId:3
    }
    Pad {
        id: pad4
        padId:4
    }
    Pad {
        id: pad5
        padId:5
    }


}


