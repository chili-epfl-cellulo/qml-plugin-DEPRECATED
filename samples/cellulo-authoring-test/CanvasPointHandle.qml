import QtQuick 2.0

Item {

    id: handle
    width: 12
    height:12
    property bool isFirstPoint: false
    property var canvasPoints
    property int pointPosition: 0
    property var canvas
    property string zoneColor: parent.zoneColor


    onIsFirstPointChanged: {
        handle.width=16;
        handle.height=16;
        handleMouseArea.cursorShape=Qt.PointingHandCursor;
    }

    signal firstPointClicked();
    signal onCanvasPointsUpdated();
    signal inPoint(bool inside)


    onXChanged: {
        canvasPoints[pointPosition].x = x;
        onCanvasPointsUpdated()
        handle.canvas.requestPaint();
    }

    onYChanged: {
        canvasPoints[pointPosition].y = y;
        onCanvasPointsUpdated()
        handle.canvas.requestPaint();
    }

    Rectangle {
        id: handleSprite
        width: parent.width
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin: -parent.width / 2
        anchors.top: parent.top
        anchors.topMargin: -parent.height / 2
        color: parent.zoneColor
        radius: 2
        border.width: 3
        border.color: Qt.darker(parent.zoneColor, 1.6)

        MouseArea {
            id: handleMouseArea
            cursorShape:Qt.CrossCursor
            anchors.fill: parent
            hoverEnabled: true
            drag.target: handle
            propagateComposedEvents: true


            onClicked: {
                if(isFirstPoint){
                    firstPointClicked();
                }
            }

            onEntered: inPoint(true)
            onExited: inPoint(false)

        }

    }

}
