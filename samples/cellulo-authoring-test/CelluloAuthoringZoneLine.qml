import QtQuick 2.0
import Cellulo 1.0

Item {
    id: celluloZoneLineHandler
    property bool isDrawing: false
    property string zoneColor: "#00BBCC"
    property var zoneTypesSelected: []
    //property var inner:
    //property var border:
    property var distance: lineDistance

    anchors.fill: parent
    property var canvasPoints: []
    property var canvas: ({})

    property int zoneIndex: -1
    property string zoneName: ""
    property real zoneMarginThickeness: 0
    property real x1Line: 0
    property real y1Line: 0
    property real x2Line: 0
    property real y2Line: 0

    signal drawingFinished()
    signal inZone(int zoneIndex, bool inside)

    function onPositionChanged(mouseArea){

    }

    function onClickedChanged(mouseArea){
        var component = Qt.createComponent("qrc:/CanvasPointHandle.qml");
        canvasPoints[canvasPoints.length] = Qt.point(mouseArea.x, mouseArea.y);
        if(!isDrawing){
            canvas.canvasPointsList[zoneIndex] = {points: canvasPoints, fillStyle: zoneColor}
            isDrawing = true;
        }
        var handle = component.createObject(celluloZoneLineHandler, {
                                                "id": "temp",
                                                "x": mouseArea.x,
                                                "y": mouseArea.y,
                                                "canvasPoints": canvasPoints,
                                                "pointPosition": canvasPoints.length-1,
                                                "canvas": canvas});
        //TODO WHEN TO DISCONNECT??
        handle.onCanvasPointsUpdated.connect(celluloZoneLineHandler.updateCanvasPoints)
        handle.inPoint.connect(celluloZoneLineHandler.reactInPoint)


        if (canvasPoints.length===2) {
            zoneColor = "#008390"
            updateCanvasPoints()
            drawingFinished();
        }

        canvas.requestPaint();

    }

    function reactInPoint(inside){
        if(inside){
            zoneColor = "#00c9dd"
        }else{
            zoneColor = "#008390"
        }
        inZone(zoneIndex, inside)
    }

    function updateCanvasPoints() {
        if(canvasPoints.length===2){
            x1Line = canvasPoints[0].x
            y1Line = canvasPoints[0].y
            x2Line = canvasPoints[1].x
            y2Line = canvasPoints[1].y
        }
    }


    onZoneColorChanged: {
        canvas.canvasPointsList[zoneIndex].fillStyle = zoneColor
        canvas.requestPaint()
    }

    function zoneCenter(){
        return {x:(x1Line + x2Line)/2, y:(y1Line + y2Line)/2}
    }

    CelluloZoneLineDistance{
        id: lineDistance
        name: "distance_" + zoneName
        marginThickeness: 0
        x1: x1Line
        y1: y1Line
        x2: x2Line
        y2: y2Line
    }

}
