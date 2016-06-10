import QtQuick 2.0
import Cellulo 1.0

Item {
    id: celluloZoneRectangleHandler
    property bool isDrawing: false
    property string zoneColor: "#00BBCC"
    property var qmlVisualRepresentation: rectangleVisualRepresentation
    property var zoneTypesSelected: []
    property var inner: rectangleInner
    property var border: rectangleBorder
    //property var distance:

    property var canvas: ({})

    property int zoneIndex: -1
    property string zoneName: ""
    property real zoneMarginThickeness: 0
    property real xRectangle: 0
    property real yRectangle: 0
    property real widthRectangle: 0
    property real heightRectangle: 0

    signal drawingFinished()
    signal inZone(int zoneIndex, bool inside)

    function onPositionChanged(mouseArea){
        if(isDrawing){
            var changeInWidth = mouseArea.x - xRectangle
            var hangeInHeight = mouseArea.y - yRectangle
            //TODO check why test
            if(changeInWidth >= 0 && hangeInHeight >= 0){
                celluloZoneRectangleHandler.widthRectangle = changeInWidth
                celluloZoneRectangleHandler.heightRectangle = hangeInHeight
            }
        }
    }

    function onClickedChanged(mouseArea){
        console.log("State isdrawing" + isDrawing)
        if(!isDrawing){
            xRectangle = mouseArea.x
            yRectangle = mouseArea.y
            widthRectangle = 0
            heightRectangle = 0
        } else{
            celluloZoneRectangleHandler.drawingFinished()
            zoneColor = "#008390"
            enableDragging()
        }

        isDrawing = !isDrawing

    }

    function enableDragging(){
        handleMouseArea.hoverEnabled = true
        handleMouseArea.enabled = true
    }


    Rectangle{
        id: rectangleVisualRepresentation
        x: xRectangle;
        y: yRectangle;
        height: heightRectangle;
        width: widthRectangle;
        color: zoneColor;
        border.color: Qt.darker(zoneColor, 1.6)
        border.width: 1;


        MouseArea {
            id: handleMouseArea
            cursorShape:Qt.CrossCursor
            anchors.fill: parent
            drag.target: rectangleVisualRepresentation
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
            xRectangle = x
            canvas.requestPaint()
        }
        onYChanged: {
            yRectangle = y
            canvas.requestPaint()
        }
    }

    function zoneCenter(){
        return {x: (xRectangle + (xRectangle+ widthRectangle))/2, y: (yRectangle + (yRectangle+ heightRectangle))/2}
    }

    CelluloZoneRectangleInner{
        id: rectangleInner
        name: "inner_" + zoneName
        marginThickeness: 0
        x: xRectangle
        y: yRectangle
        width: widthRectangle
        height: heightRectangle
    }

    CelluloZoneRectangleBorder{
        id: rectangleBorder
        name: "border_" + zoneName
        marginThickeness: zoneMarginThickeness
        x: xRectangle
        y: yRectangle
        width: widthRectangle
        height: heightRectangle
    }

}
