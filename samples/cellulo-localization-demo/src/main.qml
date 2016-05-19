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
            onConnectRequested: robotComm.macAddr = selectedAddress
            onDisconnectRequested: robotComm.disconnectFromServer()
            connected: robotComm.connected
            connecting: robotComm.connecting
        }
    }

    Rectangle{
        id: page
        anchors.top: addressBox.bottom
        anchors.left: parent.left
        anchors.margins: robotHalf

        property real scaleCoeff: Math.min((Screen.width*0.8)/210, (Screen.height*0.8 - addressBox.height)/297)
        property real robotHalf: 60*scaleCoeff/2

        width: 210*scaleCoeff
        height: 297*scaleCoeff
        color: "#EEEEEE"
        border.color: "black"
        border.width: 2
        radius: 5

        Image{
            property real gridSpacing: 0.5

            source: robotComm.kidnapped ? "../assets/redHexagon.svg" : "../assets/greenHexagon.svg"
            rotation: -robotComm.theta //QML wants clockwise angle for some reason
            x: robotComm.x*gridSpacing*parent.scaleCoeff - width/2
            y: robotComm.y*gridSpacing*parent.scaleCoeff - height/2
            sourceSize.width: 55*parent.scaleCoeff
            sourceSize.height: 60*parent.scaleCoeff
        }
    }

    Item{ //Dummy item to provide margin for bottom
        anchors.top: page.bottom
        anchors.margins: page.robotHalf
    }

    Item{ //Dummy item to provide margin for right
        anchors.left: page.right
        anchors.margins: page.robotHalf
    }

    CelluloBluetooth{
        id: robotComm
        onConnectedChanged:{
            if(connected)
                setPoseBcastPeriod(0);
        }
    }
}
