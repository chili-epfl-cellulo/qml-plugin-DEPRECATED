import QtQuick 2.0
import Cellulo 1.0

Item {
    id: celluloZoneCircleHandler
    property bool isDrawing: false
    property string zoneColor: "#00BBCC"
    property var qmlVisualRepresentation: circleVisualRepresentation
    property var zoneTypesSelected: []
    property var inner: circleInner
    property var border: circleBorder
    //property var distance

    property var canvas: ({})

    property int zoneIndex: -1
    property string zoneName: ""
    property real zoneMarginThickeness: 0
    property real xCircle: 0
    property real yCircle: 0
    property real rCircle: 0

    signal drawingFinished()
    signal inZone(int zoneIndex, bool inside)

    function onPositionChanged(mouseArea){
        if(isDrawing){
            celluloZoneCircleHandler.rCircle =  Math.sqrt(Math.abs(mouseArea.x - xCircle)*Math.abs(mouseArea.x - xCircle) + Math.abs(mouseArea.y - yCircle)*Math.abs(mouseArea.y - yCircle))
        }
    }

    function onClickedChanged(mouseArea){
        console.log("State isdrawing" + isDrawing)
        if(!isDrawing){
            xCircle = mouseArea.x
            yCircle = mouseArea.y
            rCircle = 0
        } else{
            celluloZoneCircleHandler.drawingFinished()
            zoneColor = "#008390"
            enableDragging()
        }

        isDrawing = !isDrawing

    }

    function enableDragging(){
        handleMouseArea.hoverEnabled = true
        handleMouseArea.enabled = true
    }

    function zoneCenter(){
        return {x: xCircle, y: yCircle}
    }

//TODO resolve: qrc:/CelluloAuthoringZoneCircle.qml:56:5: QML Rectangle: Binding loop detected for property "y"
    Rectangle{
        id: circleVisualRepresentation
        x: xCircle - rCircle;
        y: yCircle - rCircle;
        height: rCircle * 2;
        width: rCircle * 2;
        color: zoneColor;
        border.color: Qt.darker(zoneColor, 1.6)
        border.width: 1;
        radius: rCircle;


        MouseArea {
            id: handleMouseArea
            cursorShape:Qt.CrossCursor
            anchors.fill: parent
            drag.target: circleVisualRepresentation
            enabled: false
            hoverEnabled: false
            propagateComposedEvents: true

            onEntered: {
                zoneColor = "#00c9dd"
                inZone(zoneIndex, true)
            }
            onExited: {
                zoneColor = "#008390"
                inZone(zoneIndex, false)

            }
        }



        onXChanged: {
            xCircle = x + rCircle
            canvas.requestPaint()
        }
        onYChanged: {
            yCircle = y + rCircle
            canvas.requestPaint()
        }

    }

    CelluloZoneCircleInner{
        id: circleInner
        name: "inner_" + zoneName
        marginThickeness: 0
        x: xCircle
        y: yCircle
        r: rCircle
    }

    CelluloZoneCircleBorder{
        id: circleBorder
        name: "border_" + zoneName
        marginThickeness: zoneMarginThickeness
        x: xCircle
        y: yCircle
        r: rCircle
    }



}


