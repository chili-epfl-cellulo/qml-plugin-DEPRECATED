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

        MacAddrSelector{
            addresses: [
                "00:06:66:74:40:D2",
                "00:06:66:74:40:D4",
                "00:06:66:74:40:D5",
                "00:06:66:74:40:DB",
                "00:06:66:74:40:DC",
                "00:06:66:74:40:E4",
                "00:06:66:74:40:EC",
                "00:06:66:74:40:EE",
                "00:06:66:74:41:04",
                "00:06:66:74:41:14",
                "00:06:66:74:41:4C",
                "00:06:66:74:43:00",
                "00:06:66:74:46:58",
                "00:06:66:74:46:60",
                "00:06:66:74:48:A7"
            ]
            onConnectRequested: cellulo1.robotComm.macAddr = selectedAddress
            onDisconnectRequested: cellulo1.robotComm.disconnectFromServer()
            connected: cellulo1.robotComm.connected
            connecting: cellulo1.robotComm.connecting
        }
    }

    GroupBox {
        id: locomotionBox
        title: "Locomotion"

        anchors.top:  addressBox.bottom

        Column{
            spacing: 5

            Row{
                Label{
                    text: "X:"
                }
                TextField{
                    id: xOutputField
                    placeholderText: "X Output"
                }
            }

            Row{
                Label{
                    text: "Y:"
                }
                TextField{
                    id: yOutputField
                    placeholderText: "Y Output"
                }
            }

            Row{
                Label{
                    text: "Theta:"
                }
                TextField{
                    id: thetaOutputField
                    placeholderText: "Theta Output"
                }
            }

            Button{
                text: "Local"
                onClicked: {
                    locomotionLoop.stop();
                    cellulo1.setLocalSpeeds(parseFloat(xOutputField.text), parseFloat(yOutputField.text), parseFloat(thetaOutputField.text));
                }
            }

            Button{
                text: "Global"
                onClicked: {
                    if(!locomotionLoop.running)
                        locomotionLoop.start();
                }
            }

            Timer{
                id: locomotionLoop
                interval: 100
                repeat: true
                onTriggered: cellulo1.setGlobalSpeeds(parseFloat(xOutputField.text), parseFloat(yOutputField.text), parseFloat(thetaOutputField.text));
            }
        }
    }

    Item {
        id: playground

        // Zones playground-JourneeClasses
        property var zones: []

        property real gridSize: 0.508 //in mmm
        property real widthmm: 210
        property real heightmm: 297
    }

    CelluloRobot{
        id: cellulo1
        robotId: 1
        playground: playground
    }
}
