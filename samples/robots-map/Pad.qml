import QtQuick 2.0

Rectangle {

    property int padId: 0
    width: 16
    height: 16
    color: "black"
    radius: 8
    border.width:1

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenterOffset: Math.cos(2 * padId * Math.PI / 6) * 20
    anchors.verticalCenterOffset: Math.sin(2 * padId * Math.PI / 6) * 20

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: parent.parent.publish("T%1".arg(parent.padId));
    }
}


