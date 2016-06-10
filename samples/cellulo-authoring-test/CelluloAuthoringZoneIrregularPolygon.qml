import QtQuick 2.0
import Cellulo 1.0

Item {
    id: celluloZoneIrrPolygnonHandler
    property bool isDrawing: false
    property string zoneColor: "#00BBCC"
    property var zoneTypesSelected: []
    property var inner: irrPolygonInner
    //property var border:
    //property var distance:

    anchors.fill: parent
    property var canvasPoints: []
    property var canvas: ({})
    property var firstPointHandle: ({})

    property int zoneIndex: -1
    property string zoneName: ""
    property real zoneMarginThickeness: 0
    property var verticesIrrPolygon: []
    property bool inside: false

    signal drawingFinished()
    signal inZone(int zoneIndex, bool inside)

    function onPositionChanged(mouseArea){
        inside = isPointInPoly(mouseArea.x, mouseArea.y)
    }

    onInsideChanged: {
        if(inside){
            zoneColor = "#00c9dd"
        }else{
            zoneColor = "#008390"
        }
        inZone(zoneIndex, inside)
    }

    function onClickedChanged(mouseArea){
        var component = Qt.createComponent("qrc:/CanvasPointHandle.qml");
        canvasPoints[canvasPoints.length] = Qt.point(mouseArea.x, mouseArea.y);
        if(!isDrawing){
            canvas.canvasPointsList[zoneIndex] = {points: canvasPoints, fillStyle: zoneColor}
            isDrawing = true;
        }
        var handle = component.createObject(celluloZoneIrrPolygnonHandler, {
                                                "id": "temp",
                                                "x": mouseArea.x,
                                                "y": mouseArea.y,
                                                "canvasPoints": canvasPoints,
                                                "pointPosition": canvasPoints.length-1,
                                                "canvas": canvas});
        //TODO WHEN TO DISCONNECT??
        handle.onCanvasPointsUpdated.connect(celluloZoneIrrPolygnonHandler.updateCanvasPoints)


        if (canvasPoints.length===1) {
            firstPointHandle = handle
            firstPointHandle.isFirstPoint = true;
            firstPointHandle.firstPointClicked.connect(celluloZoneIrrPolygnonHandler.polygonClosed)
        }
        canvas.requestPaint();

    }

    function updateCanvasPoints() {
        if(canvasPoints.length>0){
            verticesIrrPolygon = canvasPoints
        }
    }

    onZoneColorChanged: {
        canvas.canvasPointsList[zoneIndex].fillStyle = zoneColor
        canvas.requestPaint()
    }


    function polygonClosed(){
        firstPointHandle.firstPointClicked.disconnect(celluloZoneIrrPolygnonHandler.polygonClosed)
        zoneColor = "#008390"
        canvas.requestPaint()
        updateCanvasPoints();
        drawingFinished();
    }

    function isPointInPoly(pointX, pointY){
        return inner.calculate(pointX,pointY,-1)===1
    }

    function zoneCenter(){
        var l = verticesIrrPolygon.length;
        var x = 0;
        var y = 0;

        for(var i =0;  i < l; i++){
            x+=verticesIrrPolygon[i].x;
            y+=verticesIrrPolygon[i].y;
        }
        return {x: x/l, y:y/l};
    }

    CelluloZoneIrregularPolygonInner{
        id: irrPolygonInner
        name: "inner_" + zoneName
        marginThickeness: 0
        vertices: verticesIrrPolygon
    }

}
