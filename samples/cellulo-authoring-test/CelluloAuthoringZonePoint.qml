import QtQuick 2.0
import Cellulo 1.0


Item {
    id: celluloZonePointHandler
    property bool isDrawing: false
    property string zoneColor: "#00BBCC"
    property var qmlVisualRepresentation: ({})
    property var zoneTypesSelected: []
    property var distance: pointDistance

    property var canvas: ({})
    property var canvasPoints: []

    property int zoneIndex: -1
    property string zoneName: ""
    property real zoneMarginThickeness: 0
    property real xPoint: 0
    property real yPoint: 0

    signal drawingFinished()
    signal inZone(int zoneIndex, bool inside)

    function onPositionChanged(mouseArea){

    }

    function onClickedChanged(mouseArea){
        var component = Qt.createComponent("qrc:/CanvasPointHandle.qml");
        //TODO check better way to deal with dragging than using canvas
        canvasPoints[canvasPoints.length] = Qt.point(mouseArea.x, mouseArea.y);
        qmlVisualRepresentation = component.createObject(celluloZonePointHandler, {
                                                             "x": mouseArea.x,
                                                             "y": mouseArea.y,
                                                             "canvasPoints": canvasPoints,
                                                             "canvas": canvas,
                                                             "pointPosition": 0,
                                                         });
        //TODO WHEN TO DISCONNECT??
        qmlVisualRepresentation.onCanvasPointsUpdated.connect(celluloZonePointHandler.updateCanvasPoints)
        qmlVisualRepresentation.inPoint.connect(celluloZonePointHandler.reactInPoint)
        updateCanvasPoints()
        zoneColor = "#008390"
        drawingFinished();
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
        if(canvasPoints.length===1){
            xPoint = canvasPoints[0].x
            yPoint = canvasPoints[0].y
        }
    }

    function zoneCenter(){
        return {x: xPoint, y: yPoint}
    }

    CelluloZonePointDistance{
        id: pointDistance
        name: "distance_" + zoneName
        marginThickeness: 0
        x: xPoint
        y: yPoint
    }
}
