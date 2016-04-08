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

    Column{
        spacing: 5

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
            Text{
                text: "Connected?"
                color: robotComm.connected ? "green" : "red"
            }
        }

        Row{
            spacing: 5

            TextField{
                id: posePublishPeriod
                placeholderText: "ms"
            }
            Button{
                text: "Set"
                onClicked: robotComm.setPoseBcastPeriod(parseFloat(posePublishPeriod.text))
            }
        }
    }

    CelluloBluetooth{
        property real xCenter: 220
        property real yCenter: 290

        property real xVel: 0
        property real yVel: 0
        property real mass: 0.1

        id: robotComm
        onPoseChanged:{
            xVel += mass*(-(x - xCenter));
            yVel += mass*(-(y - yCenter));
            if(xVel > 150)
                xVel = 150;
            else if(xVel < -150)
                xVel = -150;
            if(yVel > 150)
                yVel = 150;
            else if(yVel < -150)
                yVel = -150;

            setGoalVelocity(xVel, yVel, 0);
        }
    }
}
