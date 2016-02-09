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
                onClicked: cellulo1.robotMacAddress =  "00:06:66:74:" + macAddrRight.text;
            }
            Text{
                text: "Connected?"
                color: cellulo1.connected ? "green" : "red"
            }
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

        width: 1050 // in mm
        height: 2400 // in mm

        property int gridLeft: 0
        property int gridRight: 209900
        property int gridTop: 0
        property int gridBottom: 479900
    }

    CelluloRobot{
        id: cellulo1
        robotId: 1
        playground: playground
    }
}
