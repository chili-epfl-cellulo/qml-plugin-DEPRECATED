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

    GroupBox {
        id: addressBox
        title: "Robot Address"

        Row{
            spacing: 5

            Label{
                text: "00:06:66:74:"
                anchors.verticalCenter: macAddrRight.verticalCenter
            }
            TextField{
                id: macAddrRight
                text: "41:14"
                placeholderText: "XX:XX"

            }
            Button {
                text: "Connect"
                onClicked: robotComm.macAddr =  "00:06:66:74:" + macAddrRight.text;
            }
        }
    }

    GroupBox {
        id: motorBox
        title: "Locomotion"
        anchors.top: addressBox.bottom

        Row{
            spacing: 5

            Column{
                Label{
                    text: "Pose goal:"
                }

                TextField{
                    id: goalPoseMaxV
                    placeholderText: "v"

                }

                TextField{
                    id: goalPoseMaxW
                    placeholderText: "w"
                }

                Button{
                    text: "Go"
                    onClicked:{
                        robotComm.currentGoal = 1;
                    }
                }
            }
        }
    }

    GroupBox {
        id: statusBox
        title: "Status"
        anchors.top: motorBox.bottom

        Column{
            spacing: 5

            Row{
                spacing: 5

                Text{
                    text: "Connected?"
                    color: robotComm.connected ? "green" : "red"
                }

                Button{
                    text: "Reset"
                    onClicked:{
                        robotComm.reset();
                    }
                }
            }
            Row{
                spacing: 5

                Text{
                    text: "Kidnapped?"
                    color: robotComm.kidnapped ? "red" : "green"
                }
                Text{
                    text: "X=" + robotComm.x.toFixed(2) + " Y=" + robotComm.y.toFixed(2) + " Theta=" + robotComm.theta.toFixed(1)
                }
            }
        }
    }

    CelluloBluetooth{
        id: robotComm
        onTimestampChanged: console.log(lastTimestamp + " " + x + " " + y + " " + theta)

        property int currentGoal: 0
        property real marginXY: 6.0
        property real marginTheta: 2.5

        property real goalPoseX1: 250
        property real goalPoseY1: 1035
        property real goalPoseT1: 0

        property real goalPoseX2: 250
        property real goalPoseY2: 2235
        property real goalPoseT2: 180

        property real goalPoseX3: 1450
        property real goalPoseY3: 2235
        property real goalPoseT3: 0

        property real goalPoseX4: 1450
        property real goalPoseY4: 1035
        property real goalPoseT4: 180

        function checkMargins(goalX, goalY, goalTheta){
            if(Math.abs(goalX - x) > marginXY)
                return false;
            if(Math.abs(goalY - y) > marginXY)
                return false;
            var thetaDiff = -goalTheta - theta;
            if(thetaDiff < -180)
                thetaDiff += 360;
            else if(thetaDiff > 180)
                thetaDiff += 360;
            if(Math.abs(thetaDiff) > marginTheta)
                return false;
            return true;
        }

        function checkOrientation(checkTheta){
            var thetaDiff = -checkTheta - theta;
            if(thetaDiff < -180)
                thetaDiff += 360;
            else if(thetaDiff > 180)
                thetaDiff += 360;
            if(Math.abs(thetaDiff) > marginTheta)
                return false;
            return true;
        }

        onCurrentGoalChanged: {
            switch(currentGoal){
                 case 1:
                     robotComm.setGoalPose(goalPoseX1, goalPoseY1, 0, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     console.log("");
                     break;
                 case 2:
                     robotComm.setGoalPose(goalPoseX2, goalPoseY2, 160, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     console.log("");
                     break;
                 case 3:
                     robotComm.setGoalPose(goalPoseX3, goalPoseY3, 20, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     console.log("");
                     break;
                 case 4:
                     robotComm.setGoalPose(goalPoseX4, goalPoseY4, 160, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     console.log("");
                     break;
                 case 5:
                     robotComm.setGoalPose(goalPoseX1, goalPoseY1, 20, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     console.log("");
                     break;
            }
        }

        onPoseChanged: {
            switch(currentGoal){
                 case 1:
                     if(checkMargins(goalPoseX1, goalPoseY1, goalPoseT1)){
                         robotComm.timestampingEnabled = true;
                         currentGoal = 2;
                     }
                     break;
                 case 2:
                     if(checkOrientation(90))
                         robotComm.setGoalPose(goalPoseX2, goalPoseY2, 180, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     if(checkMargins(goalPoseX2, goalPoseY2, goalPoseT2))
                         currentGoal = 3;
                     break;
                 case 3:
                     if(checkOrientation(90))
                         robotComm.setGoalPose(goalPoseX3, goalPoseY3, 0, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     if(checkMargins(goalPoseX3, goalPoseY3, goalPoseT3))
                         currentGoal = 4;
                     break;
                 case 4:
                     if(checkOrientation(90))
                         robotComm.setGoalPose(goalPoseX4, goalPoseY4, 180, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     if(checkMargins(goalPoseX4, goalPoseY4, goalPoseT4))
                         currentGoal = 5;
                     break;
                 case 5:
                     if(checkOrientation(90))
                         robotComm.setGoalPose(goalPoseX1, goalPoseY1, 0, parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                     if(checkMargins(goalPoseX1, goalPoseY1, goalPoseT1)){
                         robotComm.timestampingEnabled = false;
                         currentGoal = 0;
                     }
                     break;
            }
        }
    }
}
