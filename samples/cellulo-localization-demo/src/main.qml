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
        title: "Robot Address"

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

    Rectangle{
        id: page
        anchors.top: addressBox.bottom
        anchors.left: parent.left
        anchors.margins: 40

        width: 210
        height: 297
        color: "#EEEEEE"
        border.color: "black"
        border.width: 2
        radius: 5

        Image{
            property real gridSpacing: 0.5

            source: robotComm.kidnapped ? "../assets/redHexagon.svg" : "../assets/greenHexagon.svg"
            rotation: -robotComm.theta //QML wants clockwise angle for some reason
            x: robotComm.x*gridSpacing - width/2
            y: robotComm.y*gridSpacing - height/2
            sourceSize.width:55
            sourceSize.height:60
        }
    }

    Item{ //Dummy item to provide margin for bottom
        anchors.top: page.bottom
        anchors.margins: 40
    }

    CelluloBluetooth{
        id: robotComm
    }
}
