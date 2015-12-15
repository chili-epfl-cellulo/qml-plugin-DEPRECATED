import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3
import QtBluetooth 5.2
import Cellulo 1.0

ApplicationWindow {
    id: window
    visible: true
    minimumHeight: height
    minimumWidth: width

    GroupBox {
        id: addressBox
        title: "Robot"

        Row{
            spacing: 5

            Label{
                text: "00:06:66:74:"
                anchors.verticalCenter: macAddrRight.verticalCenter
            }
            TextField{
                id: macAddrRight
                placeholderText: "XX:XX"
            }
            Button {
                text: "Connect"
                onClicked: robotComm.macAddr =  "00:06:66:74:" + macAddrRight.text;
            }
            Text{
                text: "Connected?"
                color: robotComm.connected ? "green" : "red"
            }
        }
    }

    GroupBox {
        id: locomotionBox
        title: "Locomotion"

        anchors.top:  addressBox.bottom

        Column{
            spacing: 5

            Text{
                text: "X=" + robotComm.x.toFixed(2) + " Y=" + robotComm.y.toFixed(2) + " Theta=" + robotComm.theta.toFixed(1)
            }

            Row{
                Label{
                    text: "X:"
                }
                TextField{
                    id: xGoalField
                    placeholderText: "X Goal"
                }
            }

            Row{
                Label{
                    text: "Y:"
                }
                TextField{
                    id: yGoalField
                    placeholderText: "Y Goal"
                }
            }

            Row{
                Label{
                    text: "Theta:"
                }
                TextField{
                    id: thetaGoalField
                    placeholderText: "Theta Goal"
                }
            }

            Button{
                text: "Go"
                onClicked:{
                    locomotionLoop.xGoal = parseFloat(xGoalField.text);
                    locomotionLoop.yGoal = parseFloat(yGoalField.text);
                    locomotionLoop.thetaGoal = parseFloat(thetaGoalField.text);
                    if(!locomotionLoop.running)
                        locomotionLoop.start();
                }
            }

            Button{
                text: "Test"
                onClicked: robotComm.setGoalPose(670, 1818, 0);
            }
        }
    }

    Timer{
        property real xGoal: 0
        property real yGoal: 0
        property real thetaGoal: 0

        property real pXY: 25.0
        property real pTheta: 75.0
        property real marginXY: 10.0
        property real marginTheta: 3.0

        property int motorTargetMax: 0xFFF
        property int motorTargetMin: 600

        property int motor1Target: 0
        property int motor2Target: 0
        property int motor3Target: 0

        property matrix4x4 k: Qt.matrix4x4(0.666666666666667,   0,                  0.333333333333333,  0,
                                           -0.333333333333333,  0.577350269189626,  0.333333333333333,  0,
                                           -0.333333333333333,  -0.577350269189626, 0.333333333333333,  0,
                                           0,                   0,                  0,                  1)

        id: locomotionLoop
        interval: 500
        repeat: true

        onTriggered: {
            if(Math.abs(xGoal - robotComm.x) < marginXY && Math.abs(yGoal - robotComm.y) < marginXY && Math.abs(thetaGoal - robotComm.theta) < marginTheta){
                console.log("Goal reached");
                motor1Target = 0;
                motor2Target = 0;
                motor3Target = 0;
                //stop();
            }
            else{
                var thetaDiff = -thetaGoal - robotComm.theta;
                if(thetaDiff < -180)
                    thetaDiff += 360;
                else if(thetaDiff > 180)
                    thetaDiff -= 360;
                var thetaDiffRad = thetaDiff/180*Math.PI;
                var xDiff = xGoal - robotComm.x;
                var yDiff = yGoal - robotComm.y;
                var xVelTarget = pXY*(Math.cos(-thetaDiffRad)*xDiff - Math.sin(-thetaDiffRad)*yDiff);
                var yVelTarget = pXY*(Math.sin(-thetaDiffRad)*xDiff + Math.cos(-thetaDiffRad)*yDiff);
                var thetaVelTarget = pTheta*(-thetaDiff);

                var velTarget = Qt.vector3d(xVelTarget, yVelTarget, thetaVelTarget);
                var motorTarget = k.times(velTarget);

                if(motorTarget.x > 0)
                    motorTarget.x += motorTargetMin;
                else if(motorTarget.x < 0)
                    motorTarget.x -= motorTargetMin;

                if(motorTarget.y > 0)
                    motorTarget.y += motorTargetMin;
                else if(motorTarget.y < 0)
                    motorTarget.y -= motorTargetMin;

                if(motorTarget.z > 0)
                    motorTarget.z += motorTargetMin;
                else if(motorTarget.z < 0)
                    motorTarget.z -= motorTargetMin;

                var absX = Math.abs(motorTarget.x);
                var absY = Math.abs(motorTarget.y);
                var absZ = Math.abs(motorTarget.z);
                if(absX >= absY && absX >= absZ){
                    if(absX > motorTargetMax){
                        motorTarget.x = motorTarget.x*motorTargetMax/absX;
                        motorTarget.y = motorTarget.y*motorTargetMax/absX;
                        motorTarget.z = motorTarget.z*motorTargetMax/absX;
                    }
                }
                else if(absY >= absZ && absY >= absX){
                    if(absY > motorTargetMax){
                        motorTarget.x = motorTarget.x*motorTargetMax/absY;
                        motorTarget.y = motorTarget.y*motorTargetMax/absY;
                        motorTarget.z = motorTarget.z*motorTargetMax/absZ;
                    }
                }
                else{
                    if(absZ > motorTargetMax){
                        motorTarget.x = motorTarget.x*motorTargetMax/absZ;
                        motorTarget.y = motorTarget.y*motorTargetMax/absZ;
                        motorTarget.z = motorTarget.z*motorTargetMax/absZ;
                    }
                }

                motor1Target = Math.round(motorTarget.x);
                motor2Target = Math.round(motorTarget.y);
                motor3Target = Math.round(motorTarget.z);
            }
        }
    }

    CelluloBluetooth{
        id: robotComm
        motor1Output: locomotionLoop.motor1Target
        motor2Output: locomotionLoop.motor2Target
        motor3Output: locomotionLoop.motor3Target
    }
}
