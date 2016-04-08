import QtQuick 2.0
import Qt.WebSockets 1.0

import Cellulo 1.0

Rectangle {

    property int robotId: 0
    property string name: ""
    width: 10
    height: 10
    visible:false

    // Battery state
    // -> Gets updated everytime the battery state changes. Is one of BatteryEnum members
    property int batteryState: btComm.batteryState

    // Touches
    // -> get updated everytime a touch occurs. Stores the touches over the last "touchHistoryDuration" seconds
    property var touches: []
    property int touchHistoryDuration: 5

    // Colors
    // -> sets the color of all pads. Use 'setPadColor()' to set an individual pad
    property color defaultColor: Qt.hsla(1,0,0.5,1);
    property color fullColor: "white"

    // Location
    // -> 'coords' holds the current coordinates of the robot, *normalized* from 0 to 1, with (0,0) the top left corner of the playground.
    property point coords: Qt.point(0,0)
    // -> physicalCoords holds the robot coordinates, in mm
    property point physicalCoords: Qt.point(0,0)
    // -> 'zone' holds the current zone that the robot hovers (empty string if none)
    property string zone: ""
    // holds the list of crossed zones
    property var zonesHistory: []

    // Object exposing the size of the playground and the list of active zones (to be set when creating an instance of Cellulo)
    property var playground: ({})

    // Communication-related properties
    //property url simulatorUrl: "ws://127.0.0.1:51102"
    //property url simulatorUrl: "ws://192.168.0.12:51102"
    property url simulatorUrl: "ws://192.168.1.11:51102"
    //property url simulatorUrl: "ws://10.42.0.1:51102"

    // Object exposing communications
    property alias robotComm: btComm

    //-----------------------------------------------------------------
    // Touches
    onTouchesChanged: {
        console.log("Touches on robot %1: %2".arg(robotId).arg(touches));
    }

    function addTouch(key){
        var newtouches = touches;
        newtouches.push(key);
        touches = newtouches;
        touchClearer.restart();
    }

    Timer {
        id: touchClearer
        interval: touchHistoryDuration * 1000;
        repeat: false;
        onTriggered: {
            console.log("No touches on robot %1 since 5 sec: clearing the touch history".arg(robotId));
            touches = [];
        }
    }

    //-----------------------------------------------------------------
    // Colors
    onFullColorChanged: {
        robotSimulatorSocket.publish("E" + CelluloToolkit.VisualEffectEnum.CONST_ALL + CelluloToolkit.formatColor(fullColor) + "00");
        btComm.setVisualEffect(CelluloToolkit.VisualEffectEnum.CONST_ALL, fullColor, 0);
    }

    function setPadColor(pad, color) {
        robotSimulatorSocket.publish("E" + CelluloToolkit.VisualEffectEnum.CONST_SINGLE + CelluloToolkit.formatColor(color) + "0" + pad);
        btComm.setVisualEffect(CelluloToolkit.VisualEffectEnum.CONST_SINGLE, color, pad);
    }

    function alert(color, nbtimes) {
        nbtimes = typeof nbtimes !== 'undefined' ? nbtimes : 1;

        robotSimulatorSocket.publish("E" + CelluloToolkit.VisualEffectEnum.ALERT_ALL + CelluloToolkit.formatColor(color) + "0" + nbtimes);
        btComm.setVisualEffect(CelluloToolkit.VisualEffectEnum.ALERT_ALL, color, nbtimes);
    }

    function waiting(color) {
        robotSimulatorSocket.publish("E" + CelluloToolkit.VisualEffectEnum.WAITING + CelluloToolkit.formatColor(color) + "00");
        btComm.setVisualEffect(CelluloToolkit.VisualEffectEnum.WAITING, color,0);
    }

    function blink(color) {
        robotSimulatorSocket.publish("E" + CelluloToolkit.VisualEffectEnum.BLINK + CelluloToolkit.formatColor(color) + "09");
        btComm.setVisualEffect(CelluloToolkit.VisualEffectEnum.BLINK, color, 0);
    }

    function pulse(color) {
        robotSimulatorSocket.publish("E" + CelluloToolkit.VisualEffectEnum.PULSE + CelluloToolkit.formatColor(color) + "09");
        btComm.setVisualEffect(CelluloToolkit.VisualEffectEnum.PULSE, color, 0);
    }

    //-----------------------------------------------------------------
    // Location
    // Check if the robot hovers an active zone, and update accordingly "zone"
    function checkZone() {
        var zones = playground.zones;

        for (var i=0; i < zones.length; i++) {
            if (CelluloToolkit.isPointInPoly(zones[i]["path"], coords)) {

                if (zones[i]["name"] !== zone) {
                    console.debug("Robot %1 enters zone %2".arg(robotId).arg(zones[i]["name"]));
                    zone = zones[i]["name"];

                    if(zonesHistory[zonesHistory.length - 1] !== zone) {
                        var zonesHistoryTmp = zonesHistory;
                        zonesHistoryTmp.push(zone);
                        zonesHistory = zonesHistoryTmp;
                    }

                }
                return zones[i]["name"];
            }
        }
        if (zone !== "") {
            zone = "";
            console.debug("Robot %1 leaves zone %2".arg(robotId).arg(zone));
        }
        return "";

    }

    //-----------------------------------------------------------------
    // Locomotion

    // DEPRECATE
    property real motorMax: 0xFFF
    property real motorMin: 0x500
    property matrix4x4 k_inv: Qt.matrix4x4(
        0.666666666666667,  0.0,                0.333333333333333,  0,
        -0.333333333333333, 0.577350269189626,  0.333333333333333,  0,
        -0.333333333333333, -0.577350269189626, 0.333333333333333,  0,
        0,                  0,                  0,                  1
    );

    // DEPRECATE
    // Set locomotion outputs (corresponds roughly to goal speeds) in local frame of reference; arguments are between -1.0 and 1.0
    function setLocalSpeeds(vx, vy, vtheta){
        if(vx > 1)
            vx = 1;
        else if(vx < -1)
            vx = -1;
        if(vy > 1)
            vy = 1;
        else if(vy < -1)
            vy = -1;
        if(vtheta > 1)
            vtheta = 1;
        else if(vtheta < -1)
            vtheta = -1;

        var localSpeeds = Qt.vector4d(vx, vy, vtheta, 1);
        var motorSpeeds = k_inv.times(localSpeeds);

        var maximum = Math.max(Math.abs(motorSpeeds.x), Math.abs(motorSpeeds.y), Math.abs(motorSpeeds.z));
        if(maximum > 1.0){
            motorSpeeds.x /= maximum;
            motorSpeeds.y /= maximum;
            motorSpeeds.z /= maximum;
        }

        if(motorSpeeds.x > 0)
            motorSpeeds.x = motorSpeeds.x*(motorMax - motorMin) + motorMin;
        else if(motorSpeeds.x < 0)
            motorSpeeds.x = motorSpeeds.x*(motorMax - motorMin) - motorMin;

        if(motorSpeeds.y > 0)
            motorSpeeds.y = motorSpeeds.y*(motorMax - motorMin) + motorMin;
        else if(motorSpeeds.y < 0)
        motorSpeeds.y = motorSpeeds.y*(motorMax - motorMin) - motorMin;

        if(motorSpeeds.z > 0)
            motorSpeeds.z = motorSpeeds.z*(motorMax - motorMin) + motorMin;
        else if(motorSpeeds.z < 0)
        motorSpeeds.z = motorSpeeds.z*(motorMax - motorMin) - motorMin;

        robotComm.setAllMotorOutputs(motorSpeeds.x, motorSpeeds.y, motorSpeeds.z);
    }

    // DEPRECATE
    // Set locomotion outputs (corresponds roughly to goal speeds) in global frame of reference; arguments are between -1.0 and 1.0
    function setGlobalSpeeds(vx, vy, vtheta){
        var vxGlobal = Math.cos(rotation/180*Math.PI)*vx - Math.sin(rotation/180*Math.PI)*vy;
        var vyGlobal = Math.sin(rotation/180*Math.PI)*vx + Math.cos(rotation/180*Math.PI)*vy;
        setLocalSpeeds(vxGlobal, vyGlobal, vtheta);
    }

    //-----------------------------------------------------------------
    // Transport

    // Simulator
    WebSocket {
        id: robotSimulatorSocket
        url: simulatorUrl
        active: false

        function publish(message) {
            if(WebSocket.Open)
                sendTextMessage(message);
        }

        onTextMessageReceived: {
            //console.debug("Robot %1: Received message: %2".arg(robotId).arg(message));
            parseSimulatorResponse(message);
        }

        onStatusChanged: {
            if (robotSimulatorSocket.status == WebSocket.Error) {
                console.error(qsTr("Client error: %1").arg(robotSimulatorSocket.errorString));
            } else if (robotSimulatorSocket.status == WebSocket.Open) {
                robotSimulatorSocket.sendTextMessage("CONNECTED%1".arg(robotId));
            } else if (robotSimulatorSocket.status == WebSocket.Closed) {
                console.log(qsTr("Client socket closed."));
            }
        }
    }

    function parseSimulatorResponse(message) {

        var matched = false;

        for (var cmd in CelluloToolkit.commands) {
            var re = CelluloToolkit.commands[cmd];
            var match = re.exec(message);
            re.lastIndex = 0;

            if (match) {
                matched = true;
                switch(cmd) {
                    case "ack":
                        break;
                    case "rebootcomplete":
                        console.log("Robot %1 completed its reboot".arg(robotId))
                        break;
                    case "wakeupcomplete":
                        console.log("Robot %1 completed its wake-up".arg(robotId))
                        break;
                    case "goingtosleep":
                        console.log("Robot %1 going to sleep (user request)".arg(robotId))
                        break;
                    case "goingtosleepbattery":
                        console.log("Robot %1 going to sleep (low battery)".arg(robotId))
                        break;
                    case "battery":
                        console.warning("Command <%1> not yet implemented".arg(cmd))
                        break;
                    case "touch":
                        addTouch(match[1]*1);
                        break;
                    case "longtouch":
                        console.warning("Command <%1> not yet implemented".arg(cmd))
                        break;
                    case "released":
                        console.warning("Command <%1> not yet implemented".arg(cmd))
                        break;
                    case "pose":
                        var gridCoords = CelluloToolkit.grid2mm(match[1], match[2], playground);
                        physicalCoords.x = gridCoords.x;
                        physicalCoords.y = gridCoords.y;
                        coords.x = gridCoords.x/playground.width;
                        coords.y = gridCoords.y/playground.height;
                        rotation = match[3]*1;
                        console.log("Robot %1 at (%2, %3)".arg(robotId).arg(coords.x).arg(coords.y));
                        checkZone();
                        break;
                    case "kidnapped":
                        console.warning("Command <%1> not yet implemented".arg(cmd))
                }
            }
            if(matched) break;
        }

        if(!matched) {
            console.warn("Robot %1 received unknown message [%2]".arg(robotId).arg(message))
        }
    }

    // Real robot
    CelluloBluetooth{
        id: btComm

        //Signals
        onBootCompleted:        console.log("Robot " + robotId + " completed its (re)boot")
        onWokeUp:               console.log("Robot " + robotId + " completed its wake-up")
        onShuttingDown:         console.log("Robot " + robotId + " going to sleep (user request)")
        onBatteryStateChanged:  console.log("Robot " + robotId + " battery state changed to: " + batteryState)

        onTouchBegan:{
            console.log("Robot " + robotId + " key " + key + "was touched");
            addTouch(key);
        }
        onLongTouch:            console.log("Robot " + robotId + " key " + key + "was long touched")
        onTouchReleased:        console.log("Robot " + robotId + " key " + key + "was released")
        onPoseChanged:{
            var gridCoords = CelluloToolkit.grid2mm(x*100, y*100, playground);
            physicalCoords.x = gridCoords.x;
            physicalCoords.y = gridCoords.y;

            var newcoordx=gridCoords.x/playground.width;
            var newcoordy=gridCoords.y/playground.height;
            var newrotation = theta;

            // Filter out minor pose update
            if (  Math.abs(coords.x - newcoordx) > 0.01
                    || Math.abs(coords.y - newcoordy) > 0.01
                    || Math.abs(parent.rotation - newrotation) > 2) // degrees
            {
                coords.x = newcoordx;
                coords.y = newcoordy;
                parent.rotation = newrotation;
                console.log("Robot %1 at (%2, %3), theta: %4".arg(robotId).arg(coords.x).arg(coords.y).arg(rotation));
            }
            checkZone();
        }
        onKidnappedChanged: {
            if(kidnapped)
                console.log("Robot " + robotId + " was kidnapped");
            else
                console.log("Robot " + robotId + " was returned on paper");
        }

        onConnectedChanged: {
            if(connected) {
                console.log("ROBOT %1 IS CONNECTED".arg(robotId));
                parent.fullColor = parent.defaultColor;
                parent.alert(Qt.rgba(0.7,0.5,0,1),3);
            }
            else {
                console.warn("ROBOT %1 DISCONNECTED".arg(robotId));
            }
        }
    }
}
