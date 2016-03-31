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
                text: "43:00"
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
            Button {
                text: "Reset"
                onClicked: robotComm.reset();
            }
            Button {
                text: "Shutdown"
                onClicked: robotComm.shutdown();
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
                    text: "207"
                    placeholderText: "X Goal"
                }
            }

            Row{
                Label{
                    text: "Y:"
                }
                TextField{
                    id: yGoalField
                    text: "292"
                    placeholderText: "Y Goal"
                }
            }

            Row{
                Label{
                    text: "Theta:"
                }
                TextField{
                    id: thetaGoalField
                    text: "30"
                    placeholderText: "Theta Goal"
                }
            }

            Row{
                Label{
                    text: "Linear max velocity:"
                }
                TextField{
                    id: linMaxVelField
                    text: "150"
                    placeholderText: "Linear Max Velocity (mm/s)"
                }
            }

            Row{
                Label{
                    text: "Theta:"
                }
                TextField{
                    id: angMaxVelField
                    text: "5"
                    placeholderText: "Angular Max Velocity (rad/s)"
                }
            }

            Button{
                text: "Go"
                onClicked: robotComm.setGoalPose(
                               parseFloat(xGoalField.text), parseFloat(yGoalField.text), parseFloat(thetaGoalField.text),
                               parseFloat(linMaxVelField.text), parseFloat(angMaxVelField.text));
            }
        }
    }

    CelluloBluetooth{
        id: robotComm
        onConnectedChanged: {
            if(connected)
                setVisualEffect(0, "#0F0F0F", 0);
        }
        onBootCompleted: setVisualEffect(0, "#0F0F0F", 0);
    }
}
