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
    minimumHeight: 100
    minimumWidth: 200

    GroupBox {
        id: addressBox
        title: "CelluloZoneEngineTest"

        Column{
            Row{
                spacing: 5

                Label{
                    text: "00:06:66:74:"
                    anchors.verticalCenter: macAddrRight1.verticalCenter
                }
                TextField{
                    id: macAddrRight1
                    placeholderText: "XX:XX"
                }
                Button {
                    text: "Connect"
                    onClicked: robotComm1.macAddr =  "00:06:66:74:" + macAddrRight1.text;
                }
                Text{
                    text: "Connected?"
                    color: robotComm1.connected ? "green" : "red"
                }
            }

            Row{
                spacing: 5

                Label{
                    text: "00:06:66:74:"
                    anchors.verticalCenter: macAddrRight2.verticalCenter
                }
                TextField{
                    id: macAddrRight2
                    placeholderText: "XX:XX"
                }
                Button {
                    text: "Connect"
                    onClicked: robotComm2.macAddr =  "00:06:66:74:" + macAddrRight2.text;
                }
                Text{
                    text: "Connected?"
                    color: robotComm2.connected ? "green" : "red"
                }
            }
        }



    }

    Row{
        id: topBox
        anchors.top: addressBox.bottom

        Column{
            spacing: 5

            Row{
                ComboBox {
                    id: combobox
                    currentIndex: 0
                    model: ListModel {
                        id: cbItems
                        ListElement { text: "Select a zone Type"}
                        ListElement { text: "CirlceInner"}
                        ListElement { text: "CircleBorder"}
                        ListElement { text: "RectangleInner" }
                    }
                    width: 200
                    onCurrentIndexChanged: {
                        switch(combobox.currentIndex) {
                        case 1:
                            console.debug("CircleInner selected");
                            circleInner.visible = true
                            circleBorder.visible = false
                            rectangleInner.visible = false
                            rectangleBorder.visible = false
                            break;
                        case 2:
                            console.debug("CircleBorder selected");
                            circleInner.visible = false
                            circleBorder.visible = true
                            rectangleInner.visible = false
                            rectangleBorder.visible = false
                            break;
                        case 3:
                            console.debug("RectangleInner selected");
                            circleInner.visible = false
                            circleBorder.visible = false
                            rectangleInner.visible = true
                            rectangleBorder.visible = false
                            break;
                        default:
                            console.debug("No Action");
                        }
                    }
                }
                Button {
                    id: buttonAddChild
                    text: "add new child"
                    property var childTypeToBeAdd: combobox.currentIndex;

                    onClicked: {
                        console.log("QML: Number children before", zoneE.children.length)

                        var properties = {};

                        switch(combobox.currentIndex) {
                        case 1:
                            console.debug("CircleInner creation");
                            var r1 = parseFloat(rCircleInner.text)
                            var x1 = parseFloat(xCircleInner.text)
                            var y1 = parseFloat(yCircleInner.text)
                            if(!isNaN(r1) && !isNaN(x1) && !isNaN(y1)){
                                properties["name"] = "zone" + zoneE.children.length
                                properties["r"] = r1;
                                properties["x"] = x1;
                                properties["y"] = y1;
                                zoneE.addNewZoneFromQML(CelluloZoneTypes.CIRCLEINNER, properties)
                            }
                            break;
                        case 2:
                            console.debug("CircleBorder creation");
                            var r2 = parseFloat(rCircleBorder.text)
                            var x2 = parseFloat(xCircleBorder.text)
                            var y2 = parseFloat(yCircleBorder.text)
                            var margin2 = parseFloat(marginCircleBorder.text)
                            if(!isNaN(r2) && !isNaN(x2) && !isNaN(y2) && !isNaN(margin2)){
                                properties["name"] = "zone" + zoneE.children.length
                                properties["r"] = r2;
                                properties["x"] = x2;
                                properties["y"] = y2;
                                properties["marginThickeness"] = margin2;
                                zoneE.addNewZoneFromQML(CelluloZoneTypes.CIRCLEBORDER, properties)
                            }

                            break;
                        case 3:
                            console.debug("RectangleInner creation");
                            var x3 = parseFloat(xRectangleInner.text)
                            var y3 = parseFloat(yRectangleInner.text)
                            var height3 = parseFloat(heightRectangleInner.text)
                            var width3 = parseFloat(widthRectangleInner.text)
                            if(!isNaN(x3) && !isNaN(y3) && !isNaN(height3) && !isNaN(width3)){
                                properties["name"] = "zone" + zoneE.children.length
                                properties["x"] = x3;
                                properties["y"] = y3;
                                properties["height"] = height3;
                                properties["width"] = width3;
                                zoneE.addNewZoneFromQML(CelluloZoneTypes.RECTANGLEINNER, properties)
                            }
                            break;
                        default:
                            console.debug("No Action");
                        }
                        console.log("QML: Number children after", zoneE.children.length);

                    }
                }
            }


            Row{
                spacing: 2
                id: circleInner
                visible: false
                Label{
                    text: "r:"
                }
                TextField{
                    id: rCircleInner
                    placeholderText: "0.0"
                }
                Label{
                    text: "x:"
                }
                TextField{
                    id: xCircleInner
                    placeholderText: "0.0"
                }
                Label{
                    text: "y:"
                }
                TextField{
                    id: yCircleInner
                    placeholderText: "0.0"
                }
            }
            Row{
                spacing: 2
                id: circleBorder
                visible: false
                Label{
                    text: "r:"
                }
                TextField{
                    id: rCircleBorder
                    placeholderText: "0.0"
                }
                Label{
                    text: "x:"
                }
                TextField{
                    id: xCircleBorder
                    placeholderText: "0.0"
                }
                Label{
                    text: "y:"
                }
                TextField{
                    id: yCircleBorder
                    placeholderText: "0.0"
                }
                Label{
                    text: "margin:"
                }
                TextField{
                    id: marginCircleBorder
                    placeholderText: "0.0"
                }
            }

            Row{
                spacing: 2
                id: rectangleInner
                visible: false
                Label{
                    text: "x:"
                }
                TextField{
                    id: xRectangleInner
                    placeholderText: "0.0"
                }
                Label{
                    text: "y:"
                }
                TextField{
                    id: yRectangleInner
                    placeholderText: "0.0"
                }
                Label{
                    text: "height:"
                }
                TextField{
                    id: heightRectangleInner
                    placeholderText: "0.0"
                }
                Label{
                    text: "width:"
                }
                TextField{
                    id: widthRectangleInner
                    placeholderText: "0.0"
                }


            }
            Row{
                spacing: 2
                id: rectangleBorder
                visible: false
                Label{
                    text: "x:"
                }
                TextField{
                    id: xRectangleBorder
                    placeholderText: "0.0"
                }
                Label{
                    text: "y:"
                }
                TextField{
                    id: yRectangleBorder
                    placeholderText: "0.0"
                }
                Label{
                    text: "height:"
                }
                TextField{
                    id: heightRectangleBorder
                    placeholderText: "0.0"
                }
                Label{
                    text: "width:"
                }
                TextField{
                    id: widthRectangleBorder
                    placeholderText: "0.0"
                }
                Label{
                    text: "margin:"
                }
                TextField{
                    id: marginRectangleBorder
                    placeholderText: "0.0"
                }
            }
            Row{
                Button{
                    text: programmedZone.active? "Original Zone Active" : "Original Zone Inactive"
                    onClicked: programmedZone.active = !programmedZone.active
                }
                Column{
                    Button{
                        text: "ExportZones"
                        onClicked: {
                            if(!zoneE.getAllZonesAndSaveThem(exportedZone.text)){
                                console.log("problem exporting zones")
                            }
                        }
                    }
                    Label{
                        text: "Path of exported zone"
                    }
                    TextField{
                        width:  200
                        id: exportedZone
                        placeholderText: "xxx.json"
                    }}
                Column{
                    Button {
                        text: "LoadZones"
                        onClicked: {
                            if(!zoneE.addNewZoneFromJSON(importedZone.text)){
                                console.log("problem loading zones")
                            }

                        }

                    }
                    Label{
                        text: "Path of imported zone"
                    }
                    TextField{
                        width:  200
                        id: importedZone
                        placeholderText: "xxx.json"
                    }

                }
            }




        }



    }
    Column{
        anchors.leftMargin: 40
        anchors.left: page.right
        anchors.top: page.top
        Text {
            id: textNumberOfChildren
            text: "Zone engine number of children " + zoneE.children.length
        }
        Text {
            id: textChildrenInfo
            text: printChildrenInfo()
        }
    }
    CelluloZoneEngine{
        id: zoneE
        objectName: "ZoneEngine"
        vRplaygroundWidth: page.width
        vRplaygroundHeight: page.height
        realPlaygroundWidth: 209.90278625488281
        realPlaygroundHeight: 297.03890991210938

        onChildrenChanged: {
            textNumberOfChildren.text = "Zone engine number of children " + zoneE.children.length
        }

        CelluloZoneRectangleInner{
            id: programmedZone
            height: 30
            width: 50
            x: 150
            y: 30
            name: "zone" + zoneE.children.length
        }

        Component.onCompleted: {
            console.log("CONNECTION MADE")
            zoneE.newZoneCreatedReadyForVisualization.connect(CelluloZonesDrawer.drawNewZone)
            CelluloZonesDrawer.setVisualParent(page)
        }
    }

    CelluloZoneTypes{
        id: typesEnum
    }

    function printChildrenInfo(){
        var s = "";
        for (var i = 0; i < zoneE.children.length; i++)
        {
            s += zoneE.children[i]  + "\n"
        }
        return s
    }


    Rectangle{
        id: page
        anchors.left: parent.left
        anchors.margins: robotHalf
        anchors.top: topBox.bottom

        property real realPlaygroundWidth: zoneE.realPlaygroundWidth
        property real realPlaygroundHeight: zoneE.realPlaygroundHeight
        //property real scaleCoeff: Math.min((Screen.width*0.8)/realPlaygroundWidth, (Screen.height*0.8 - topBox.height)/realPlaygroundHeight)
        property real scaleCoeff:2.6
        property real robotHalf: 60*scaleCoeff/2
        property real gridSpacing: 0.508

        width: realPlaygroundWidth*scaleCoeff
        height: realPlaygroundHeight*scaleCoeff
        color: "#EEEEEE"
        border.color: "black"
        border.width: 2
        radius: 5

        Image{
            id: robotImage
            property real gridSpacing: 0.5

            source: robotComm1.kidnapped ? "../assets/redHexagon.svg" : "../assets/greenHexagon.svg"
            rotation: -robotComm1.theta //QML wants clockwise angle for some reason
            x: robotComm1.x*gridSpacing*parent.scaleCoeff - width/2
            y: robotComm1.y*gridSpacing*parent.scaleCoeff - height/2
            sourceSize.width: 55*parent.scaleCoeff
            sourceSize.height: 60*parent.scaleCoeff
        }

        Image{
            id: robotImage2
            property real gridSpacing: 0.5
            source: robotComm2.kidnapped ? "../assets/redHexagon.svg" : "../assets/greenHexagon.svg"
            rotation: -robotComm2.theta //QML wants clockwise angle for some reason
            x: robotComm2.x*gridSpacing*parent.scaleCoeff - width/2
            y: robotComm2.y*gridSpacing*parent.scaleCoeff - height/2
            sourceSize.width: 55*parent.scaleCoeff
            sourceSize.height: 60*parent.scaleCoeff
        }

        Rectangle{
            id: centerRobotImage1
            x: robotImage.x + robotImage.width/2
            y: robotImage.y + robotImage.height/2
            height: 5
            width: 5
            color: "black"
            border.color: "black"
            border.width: 1
            radius: 2.5
        }


        Rectangle{
            id: centerRobotImage2
            x: robotImage2.x + robotImage2.width/2
            y: robotImage2.y + robotImage2.height/2
            height: 5
            width: 5
            color: "black"
            border.color: "black"
            border.width: 1
            radius: 2.5
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
            id: robotComm1

            onZonesChangesHandlerChanged: {
                var zoneConcerned = zonesChangesHandler[0]
                var value = zonesChangesHandler[1]
                switch(zoneConcerned.type){
                case CelluloZoneTypes.CIRCLEINNER:
                    console.log("IN CINNER " + value)
                    if(value === 1){
                        robotComm1.setVisualEffect(0, "blue", 0);
                    } else{
                        robotComm1.setVisualEffect(0, "white", 0);
                    }
                    break;
                case CelluloZoneTypes.CIRCLEBORDER:
                    console.log("IN CBORDER " + value)
                    if(value === 1){
                        robotComm1.setVisualEffect(7, "blue", 0);
                    } else{
                        robotComm1.setVisualEffect(0, "white", 0);
                    }
                    break;
                case CelluloZoneTypes.RECTANGLEINNER:
                    console.log("IN RINNER " + value)
                    if(value === 1){
                        robotComm1.setVisualEffect(0, "yellow", 0);
                    } else{
                        robotComm1.setVisualEffect(0, "white", 0);
                    }
                    break;
                case CelluloZoneTypes.RECTANGLEBORDER:
                    console.log("IN RBORDER " + value)
                    if(value === 1){
                        robotComm1.setVisualEffect(7, "yellow", 0);
                    } else{
                        robotComm1.setVisualEffect(0, "white", 0);
                    }
                    break;
                case CelluloZoneTypes.RECTANGLEBORDER:
                    console.log("IN RBORDER " + value)
                    if(value === 1){
                        robotComm1.setVisualEffect(7, "yellow", 0);
                    } else{
                        robotComm1.setVisualEffect(0, "white", 0);
                    }
                    break;
                case CelluloZoneTypes.IRPOLYGONINNER:
                    console.log("IN IRPINNER " + value)
                    if(value === 1){
                        robotComm1.setVisualEffect(0, "purple", 0);
                    } else{
                        robotComm1.setVisualEffect(0, "white", 0);
                    }
                    break;
                case CelluloZoneTypes.RPOLYGONINNER:
                    console.log("IN RPINNER " + value)
                    if(value === 1){
                        robotComm1.setVisualEffect(7, "purple", 0);
                    } else{
                        robotComm1.setVisualEffect(0, "white", 0);
                    }
                    break;
                default:
                    //console.log("UNKNOW IN " + zonesChangesHandler[0] + " " + value)
                    break;
                }
            }


        }

        CelluloBluetooth{
            id: robotComm2
        }

        CelluloZoneJsonHandler{
            id: jsonHandler
        }


    }
}

