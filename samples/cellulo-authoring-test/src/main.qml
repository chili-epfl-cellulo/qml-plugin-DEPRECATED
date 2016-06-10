import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3
import Cellulo 1.0


ApplicationWindow {
    id: window
    visible: true
    visibility: "Maximized"
    color: "#000"

    GroupBox{
        id: actionBar
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: base.top



        ComboBox {
            id: zoneTypeSelection
            currentIndex: 0
            anchors.left: parent.left
            width: 200
            model: ListModel {
                id: type
                ListElement { text: "Select a zone Type"}
                ListElement { text: "Circle"}
                ListElement { text: "Rectangle"}
                ListElement { text: "Line"}
                ListElement { text: "Point"}
                ListElement { text: "Regular Polygon"}
                ListElement { text: "Irregular Polygon"}

            }

            onCurrentIndexChanged: {
                switch(zoneTypeSelection.currentIndex) {
                    //TODO implement missing
                case 1:
                    console.debug("Circle selected");
                    innerSelection.visible = true
                    borderSelection.visible = true
                    //distanceSelection.visible = true
                    distanceSelection.visible = false
                    break;
                case 2:
                    console.debug("Rectangle selected");
                    innerSelection.visible = true
                    borderSelection.visible = true
                    //distanceSelection.visible = true
                    distanceSelection.visible = false
                    break;
                case 3:
                    console.debug("Line selected");
                    innerSelection.visible = false
                    //borderSelection.visible = true
                    borderSelection.visible = false
                    distanceSelection.visible = true
                    break;
                case 4:
                    console.debug("Point selected");
                    innerSelection.visible = false
                    borderSelection.visible = false
                    distanceSelection.visible = true
                    break;
                case 5:
                    console.debug("RegularPolygon selected");
                    innerSelection.visible = true
                    borderSelection.visible = true
                    distanceSelection.visible = true
                    break;
                case 6:
                    console.debug("IrregularPolygon selected");
                    innerSelection.visible = true
                    //borderSelection.visible = true
                    borderSelection.visible = false
                    //distanceSelection.visible = true
                    distanceSelection.visible = false
                    break;
                default:
                    console.debug("No Action");
                }
            }
        }

        Column{
            id: zoneSubTypeSelection
            anchors.left: zoneTypeSelection.right
            anchors.leftMargin: 20


            CheckBox {
                id: innerSelection
                style: CheckBoxStyle {
                    label: Text {
                        color: "white"
                        text: "Inner"
                    }
                }
            }
            CheckBox {
                id: borderSelection
                style: CheckBoxStyle {
                    label: Text {
                        color: "white"
                        text: "Border"
                    }
                }

                onCheckedChanged: borderSelection.checked? rowMargin.visible=true: rowMargin.visible=false
            }
            CheckBox {
                id: distanceSelection
                style: CheckBoxStyle {
                    label: Text {
                        color: "white"
                        text: "Distance"
                    }
                }
            }

            function getSubTypeSelected(){
                return [innerSelection.checked? 1:0, borderSelection.checked? 1:0, distanceSelection.checked? 1:0]
            }

            function isAnyChecked(){
                return (innerSelection.checked || borderSelection.checked || distanceSelection.checked? true:false)            }

            function reset(){
                innerSelection.checked = false
                borderSelection.checked = false
                distanceSelection.checked = false
            }
        }
        Column{
            id: thirdColumn
            anchors.left: zoneSubTypeSelection.right
            anchors.leftMargin: 20

            ComboBox {
                id: creationModeSelection
                currentIndex: 0
                width: 240
                anchors.bottomMargin: 20
                model: ListModel {
                    id: mode
                    ListElement { text: "Select a creation mode"}
                    ListElement { text: "Manual"}
                    ListElement { text: "Drawing"}
                }
            }

            Row{
                id: rowMargin
                spacing: 20
                visible: false
                Label{
                    text: "Margin"
                    color: "white"
                    anchors.verticalCenter: margin.verticalCenter
                }
                TextField{
                    id: margin
                    placeholderText: "0"
                    text: "0"

                }
            }

        }



        Button {
            id: createZoneButton
            text: "Create zone"
            anchors.left: thirdColumn.right
            anchors.leftMargin: 8
            //TODO Check if really useful, maybe make color
            onClicked: {
                //console.log("Checkboxes " + zoneSubTypeSelection.getSubTypeSelected())
                //TODO find better place to do reset
                //zoneSubTypeSelection.reset();
                //base.printZones()
                //canvas.deleteZone("zone1")
                //console.log(base.focus)

            }
        }

        Button {
            id: importJSONButton
            text: "Import JSON"
            anchors.right: exportJSONButton.left
            anchors.leftMargin: 8

            onClicked: {
                //jsonHandler.loadZones("/home/joanna/Documents/projetMA3/celluloPersonalProjectMA/allZonesJsonTest.json");
                //TODO add zones like property bar
            }
        }


        Button {
            id: exportJSONButton
            text: "Export JSON"
            anchors.right: parent.right
            anchors.leftMargin: 8

            onClicked: {
                jsonHandler.setZonesFromQML(base.getZones());
                jsonHandler.saveZones("/home/joanna/Documents/projetMA3/celluloPersonalProjectMA/saveZoneAuthoring.json");
            }
        }

    }

    Rectangle {
        id: base
        objectName: "base"
        width: parent.width
        anchors.top: actionBar.bottom
        anchors.bottom: parent.bottom
        focus: true
        color: "#000"
        property var zones: []
        property bool currentlyDrawing: false
        property var currentHandle: null
        //TODO always keep that number in case of duplica name when remaining (always cut last char for displaying + saving)
        property int numberZonesCreatedFromBeginning: 0
        property int activeZone: -1

        //Used for dragging debug purpose, see if still useful
        function printZones(){
            console.log("Zones size " + zones.length)


            for (var i in zones) {
                console.log("Zones types " + zones[i].zoneTypesSelected + ": " + (zones[i].zoneTypesSelected[0] === 1) + ","+
                            (zones[i].zoneTypesSelected[1] === 1) + "," + (zones[i].zoneTypesSelected[2] === 1));

                if(zones[i].zoneTypesSelected[0] === 1){
                    console.log("inner " + zones[i].inner);
                    if(qmltypeof(zones[i].inner, "CelluloZoneCircleInner")){
                        console.log("Circle x inner" + zones[i].inner.x)
                        console.log("Circle x vizu " + zones[i].qmlVisualRepresentation.x)

                    }
                    if(qmltypeof(zones[i].inner, "CelluloZoneRectangleInner")){
                        console.log("Rectangle x inner" + zones[i].inner.x)
                        console.log("Rectangle x vizu " + zones[i].qmlVisualRepresentation.x)

                    }
                    if(qmltypeof(zones[i].inner, "CelluloZoneIrregularPolygonInner")){
                        console.log("Irpoly x vertices" + zones[i].verticesIrrPolygon)
                        console.log("Irpoly x inner" + zones[i].inner.vertices)
                        console.log("Irpoly x vizu " + zones[i].canvasPoints)

                    }
                }
                if (zones[i].zoneTypesSelected[1] === 1){
                    console.log("border " + zones[i].border);
                }
                if (zones[i].zoneTypesSelected[2] === 1){
                    console.log("distance " + zones[i].distance);
                    if(qmltypeof(zones[i].distance, "CelluloZonePointDistance")){
                        console.log("Point x distance" + zones[i].distance.x)
                        console.log("Point x vizu " + zones[i].qmlVisualRepresentation.x)

                    }
                    if(qmltypeof(zones[i].distance, "CelluloZoneLineDistance")){
                        console.log("Line x distance" + zones[i].distance.x1)
                        console.log("Line x vizu " + zones[i].canvasPoints[0].x)
                        console.log("Line x vizu " + zones[i].canvasPoints)


                    }
                }
            }
        }

        function getZones(){
            var zonesToBeExported = []
            for (var zoneIndex in zones) {
                //TODO why key equals??
                if(zones[zoneIndex].zoneTypesSelected !== undefined ){
                    if(zones[zoneIndex].zoneTypesSelected[0] === 1){
                        console.log("inner " + zones[zoneIndex].inner);
                        zonesToBeExported.push(zones[zoneIndex].inner)

                    }
                    if (zones[zoneIndex].zoneTypesSelected[1] === 1){
                        console.log("border " + zones[zoneIndex].border);
                        zonesToBeExported.push(zones[zoneIndex].border)

                    }
                    if (zones[zoneIndex].zoneTypesSelected[2] === 1){
                        console.log("distance " + zones[zoneIndex].distance);
                        zonesToBeExported.push(zones[zoneIndex].distance)

                    }
                }
            }
            return zonesToBeExported;
        }

        //Taken from http://stackoverflow.com/questions/13923794/how-to-do-a-is-a-typeof-or-instanceof-in-qml
        function qmltypeof(obj, className) { // QtObject, string -> bool
            // className plus "(" is the class instance without modification
            // className plus "_QML" is the class instance with user-defined properties
            var str = obj.toString();
            return str.indexOf(className + "(") == 0 || str.indexOf(className + "_QML") == 0;
        }


        function checkComponentState(componentToCheck){
            if( componentToCheck.status !== Component.Ready )
            {
                if( componentToCheck.status === Component.Error ){
                    console.debug("Error:"+ componentToCheck.errorString() );
                }
                currentlyDrawing = false;
            }
        }

        function checkActiveZone(i, inside){
            if(inside){
                activeZone = i
                mouseArea.cursorShape=Qt.CrossCursor

            } else {
                if(activeZone === i){
                    //TODO find fix for problem polygon in circle
                    activeZone = -1
                }
                mouseArea.cursorShape=Qt.ArrowCursor
            }
            console.log("current active zone " + activeZone)
        }

        function zoneCreated(){
            disconnectFromHandle(currentHandle)
            //zones.push(currentHandle)
            zones[currentHandle.zoneIndex] = currentHandle
            //TODO disconnect when zone removed
            currentHandle.inZone.connect(base.checkActiveZone)
            //UGLY TRICK, find better way
            if(qmltypeof(currentHandle, "CelluloAuthoringZoneIrregularPolygon")){
                mouseArea.positionChanged.connect(currentHandle.onPositionChanged)
            }
            currentlyDrawing = false
            currentHandle = null
            numberZonesCreatedFromBeginning++
        }

        function connectToHandle(handleToConnect){
            if (handleToConnect !== null ) {
                mouseArea.clicked.connect(handleToConnect.onClickedChanged)
                mouseArea.positionChanged.connect(handleToConnect.onPositionChanged)
                handleToConnect.drawingFinished.connect(base.zoneCreated)
            } else {
                // Error Handling
                console.log("Error creating object");
                currentlyDrawing = false;
            }


        }

        function disconnectFromHandle(handleToDisconnect){
            mouseArea.clicked.disconnect(handleToDisconnect.onClickedChanged)
            mouseArea.positionChanged.disconnect(handleToDisconnect.onPositionChanged)
            handleToDisconnect.drawingFinished.disconnect(base.zoneCreated)
            handleToDisconnect.inZone.disconnect(base.checkActiveZone)

        }

        MouseArea {
            id: mouseArea


            anchors.fill: playground
            drag.target: base
            hoverEnabled: true


            onClicked: {
                canvas.requestPaint()
                console.log("number of component " + playground.children.length)
                console.log("status: " + (!base.currentlyDrawing) + "," + (creationModeSelection.currentIndex === 2) + "," + (zoneTypeSelection.currentIndex !== 0)  + "," + (zoneSubTypeSelection.isAnyChecked()))
                if((!base.currentlyDrawing) && (creationModeSelection.currentIndex === 2) &&
                        (zoneTypeSelection.currentIndex !== 0) && (zoneSubTypeSelection.isAnyChecked())){
                    var zoneTypesSelected = zoneSubTypeSelection.getSubTypeSelected()
                    console.log("Drawing")
                    base.currentlyDrawing = true;
                    var component;
                    switch(zoneTypeSelection.currentIndex) {
                    case 1:
                        console.debug("Circle selected");
                        component = Qt.createComponent("qrc:/CelluloAuthoringZoneCircle.qml");
                        base.checkComponentState(component)
                        base.currentHandle = component.createObject(playground, {
                                                                        "zoneName": "zone" + base.numberZonesCreatedFromBeginning,
                                                                        "zoneIndex": base.numberZonesCreatedFromBeginning,
                                                                        "zoneTypesSelected": zoneTypesSelected,
                                                                        "canvas": canvas})
                        break;
                    case 2:
                        console.debug("Rectangle selected");
                        component = Qt.createComponent("qrc:/CelluloAuthoringZoneRectangle.qml");
                        base.checkComponentState(component)
                        base.currentHandle = component.createObject(playground, {
                                                                        "zoneName": "zone" + base.numberZonesCreatedFromBeginning,
                                                                        "zoneIndex": base.numberZonesCreatedFromBeginning,
                                                                        "zoneTypesSelected": zoneTypesSelected,
                                                                        "canvas": canvas})
                        break;
                    case 3:
                        console.debug("Line selected");
                        component = Qt.createComponent("qrc:/CelluloAuthoringZoneLine.qml");
                        base.checkComponentState(component)
                        base.currentHandle = component.createObject(playground, {
                                                                        "zoneName": "zone" + base.numberZonesCreatedFromBeginning,
                                                                        "zoneIndex": base.numberZonesCreatedFromBeginning,
                                                                        "zoneTypesSelected": zoneTypesSelected,
                                                                        "canvas": canvas});
                        break;
                    case 4:
                        console.debug("Point selected");
                        component = Qt.createComponent("qrc:/CelluloAuthoringZonePoint.qml");
                        base.checkComponentState(component)
                        base.currentHandle = component.createObject(playground, {
                                                                        "zoneName": "zone" + base.numberZonesCreatedFromBeginning,
                                                                        "zoneIndex": base.numberZonesCreatedFromBeginning,
                                                                        "zoneTypesSelected": zoneTypesSelected,
                                                                        "canvas": canvas})
                        break;
                    case 5:
                        console.debug("RegularPolygon selected");
                        break;
                    case 6:
                        console.debug("IrregularPolygon selected");
                        component = Qt.createComponent("qrc:/CelluloAuthoringZoneIrregularPolygon.qml");
                        base.checkComponentState(component)
                        base.currentHandle = component.createObject(playground, {
                                                                        "zoneName": "zone" + base.numberZonesCreatedFromBeginning,
                                                                        "zoneIndex": base.numberZonesCreatedFromBeginning,
                                                                        "zoneTypesSelected": zoneTypesSelected,
                                                                        "canvas": canvas})
                        break;
                    default:
                        console.debug("No Action");
                    }

                    if (base.currentHandle !== null && zoneTypesSelected[1]===1){
                        base.currentHandle.zoneMarginThickeness = margin.text
                        //console.log("margin NEW"+ base.currentHandle.zoneMarginThickeness + "," + base.currentHandle.border.marginThickeness)
                    }

                    base.connectToHandle(base.currentHandle)
                    zoneSubTypeSelection.reset();
                }
            }

            onDoubleClicked: {
                if (base.activeZone !== -1) {
                    zoneNameBackground.x = mouse.x + 15;
                    zoneNameBackground.y = mouse.y + 15;
                    zoneNameEdition.text = base.zones[base.activeZone].zoneName;
                    zoneNameEdition.selectAll();
                    zoneNameEdition.activeZone = base.activeZone;
                    zoneNameBackground.visible=true;
                    zoneNameEdition.focus=true;
                }

            }


        }

        Rectangle{
            //LATER IT WILL NO LONGER BE A RECTANGLE BUT AN IMAGE
            id: playground
            property real scaleCoeff: Math.min((Screen.width*0.8)/210, (Screen.height*0.8 - actionBar.height)/297)
            //TODO move zones in playground

            width: 210*scaleCoeff
            height: 297*scaleCoeff
            color: "#EEEEEE"
            anchors.horizontalCenter: base.horizontalCenter
            anchors.verticalCenter: base.verticalCenter

            border.color: "black"
            border.width: 2
            radius: 5

        }

        Canvas{
            id: canvas
            property var canvasPointsList: []
            anchors.fill: playground


            onPaint: {
                if(canvasPointsList !== undefined){

                    var ctx = canvas.getContext('2d')
                    ctx.clearRect(0, 0, canvas.width, canvas.height);
                    //TODO check why prop equals
                    for (var prop in canvas.canvasPointsList) {
                        //console.log("prop" + prop)
                        if(canvas.canvasPointsList[prop] !== undefined ){
                            //console.log("Object item:", prop, "=", canvas.canvasPointsList[prop])
                            //console.log("Points2 " + canvas.canvasPointsList[prop].points + " fillstyle " + canvas.canvasPointsList[prop].fillStyle + " length " + canvas.canvasPointsList[prop].points.length)
                            if(canvas.canvasPointsList[prop].points !== undefined){
                                drawOneShape(canvas.canvasPointsList[prop].points, canvas.canvasPointsList[prop].fillStyle, ctx)

                            }
                        }
                    }

                    for (var zoneIndex in base.zones) {
                        var zone = base.zones[zoneIndex]
                        if(zone.zoneName !== undefined ){
                            drawZoneName(ctx, zone.zoneName, zone.zoneCenter())
                        }
                    }
                }

            }

            function drawZoneName(ctx, name, center){
                ctx.fillStyle = "rgba(0,0,0,0.7)";
                //ctx.strokeStyle = "rgba(200,200,200,0.7)";
                ctx.textAlign = "center";
                ctx.font = "20px sans-serif";
                ctx.lineWidth = 1;
                ctx.strokeText(name,center.x,center.y);
                ctx.fillText(name,center.x,center.y);

            }

            function drawOneShape(canvasPoints, canvasFillStyle, ctx){
                ctx.beginPath();
                ctx.moveTo(canvasPoints[0].x,canvasPoints[0].y);

                for(var i = 1; i <canvasPoints.length; i++){
                    ctx.lineTo(canvasPoints[i].x,canvasPoints[i].y);
                }
                ctx.lineTo(canvasPoints[0].x,canvasPoints[0].y);
                ctx.fillStyle = canvasFillStyle;
                ctx.fill();
                ctx.stroke();
                ctx.closePath();
            }

            function deleteZoneInCanvas(zoneIndex){
                if(zoneIndex in canvasPointsList){
                    delete canvas.canvasPointsList[zoneIndex];
                    canvas.requestPaint();
                }
            }
        }

        Rectangle {
            id: zoneNameBackground
            x: 0
            y: 0
            z:10
            width: 208
            height: 21
            color: "#b3ffffff"
            border.width: 2
            visible: false
            parent: playground

            TextInput {
                id: zoneNameEdition

                property int activeZone: -1

                color: "#000000"
                text: ""
                font.pointSize: 12
                visible: true
                anchors.fill: parent

                onTextChanged: {
                    if (zoneNameEdition.activeZone != -1) {
                        base.zones[activeZone].zoneName = text;
                        canvas.requestPaint();
                    }
                }

                onEditingFinished: {
                    zoneNameEdition.activeZone = -1;
                    zoneNameBackground.visible=false;
                    zoneNameEdition.text="";
                    zoneNameEdition.focus=false;
                    base.focus = true;
                }

            }
        }

        Keys.onDeletePressed: {
            if (activeZone != -1) {
                var zoneToBeDestroyed = zones[activeZone]
                if(activeZone in zones){
                    delete zones[activeZone];
                    canvas.deleteZoneInCanvas(activeZone)
                }
                canvas.requestPaint();
                //TODO check weird behaviour children length
                disconnectFromHandle(zoneToBeDestroyed)
                zoneToBeDestroyed.destroy()
                activeZone = -1;
            }
        }

        Keys.onEscapePressed: {
            window.close();
        }


    }


    CelluloZoneJsonHandler{
        id: jsonHandler
    }

}
