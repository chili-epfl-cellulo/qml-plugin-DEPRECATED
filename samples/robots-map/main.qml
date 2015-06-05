import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1

import Qt.WebSockets 1.0

Window {
    id: simulator
    width: 1600
    height: 2560
    visible: true
    //visibility: "Maximized"
    color: "#000"

    property var robots: []

    Image {
        id: playground

        source:"assets/playground.png"

        fillMode: Image.Pad
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop

        anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.width: width
        sourceSize.height: height

        property int gridLeft: 0
        property int gridRight: 209900
        property int gridTop: 0
        property int gridBottom: 479771

        MouseArea {
            id: mouseArea1
            anchors.fill: parent
            hoverEnabled: true
            onPositionChanged: {
                var x = mouse.x/playground.width
                var y = mouse.y/playground.height
                var x2 = mouse.x/playground.width;
                var y2 = mouse.y/playground.height;
                infoBox.text = "(%1cm, %2cm), (%3, %4)".arg(Number(x).toFixed(2)).arg(Number(y).toFixed(2)).arg(Number(x2).toFixed(2)).arg(Number(y2).toFixed(2))
            }
        }

        Rectangle {
            id: urlInfoBoxBackground
            x: 214
            width: 147
            height: 20
            color: "#b3ffffff"
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8

            Text {
                id: urlInfoBox
                horizontalAlignment: Text.AlignRight
                anchors.fill: parent
                font.pixelSize: 12
            }
        }

        Rectangle {
            id: infoBoxBackground
            x: 194
            y: 565
            width: 202
            height: 20
            color: "#ffffff"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8

            Text {
                id: infoBox
                horizontalAlignment: Text.AlignRight
                anchors.fill: parent
                font.pixelSize: 12
            }
        }



    Cellulo {
        id: celluloRobot1
        robotId: 1
        playground: playground
        robotMacAddress: "00:06:66:74:40:D2"
    }

    Cellulo {
        id: celluloRobot2
        robotId: 2
        playground: playground
        robotMacAddress: "00:06:66:74:40:D4"
    }

    Cellulo {
        id: celluloRobot3
        robotId: 3
        playground: playground
        robotMacAddress: "00:06:66:74:40:DC"
    }

    Cellulo {
        id: celluloRobot4
        robotId: 4
        playground: playground
        robotMacAddress: "00:06:66:74:41:04"
    }

    Cellulo {
        id: celluloRobot5
        robotId: 5
        playground: playground
        robotMacAddress: "00:06:66:74:41:14"
    }

    }
}
