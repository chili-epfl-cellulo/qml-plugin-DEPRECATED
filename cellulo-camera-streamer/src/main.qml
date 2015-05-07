import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtBluetooth 5.2

Window {
    id: window
    visible: true
    height: 300
    width: 500

    Column{
        spacing: 10

        Button {
            id: pingButton
            text: "Ping"
            onClicked: robot.sendData("PING_\n")
        }
    }

    Column{
        anchors.right: parent.right
        spacing: 10

        CelluloRobot{
            id:  robot
            macAddress:  "00:06:66:74:41:14"
        }
    }
}
