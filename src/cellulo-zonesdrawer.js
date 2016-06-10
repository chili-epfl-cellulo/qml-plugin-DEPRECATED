
var parent = null

function setVisualParent(newparent){
    parent = newparent
}

function drawNewCircleInner(rCircleInner, xCircleInner, yCircleInner, childNumber, playgroundWidth, playgroundHeight){
    xCircleInner = xCircleInner*playgroundWidth
    yCircleInner = yCircleInner*playgroundHeight
    rCircleInner = Math.sqrt(rCircleInner*playgroundWidth*playgroundHeight)

    Qt.createQmlObject(
                'import QtQuick 2.2;Rectangle{id: child' + childNumber + ';x:'+ xCircleInner +'-'+ rCircleInner +';y: '+ yCircleInner +'- '+ rCircleInner +';height: '+ rCircleInner +' * 2;width: '+ rCircleInner +' * 2;color: "blue";border.color: "black";border.width: 1;radius: '+ rCircleInner +';}',
                parent,'child' + childNumber)
}

function drawNewCircleBorder(rCircleBorder, xCircleBorder, yCircleBorder, marginCircleBorder, childNumber, playgroundWidth, playgroundHeight){
    marginCircleBorder = marginCircleBorder*playgroundWidth
    xCircleBorder = xCircleBorder*playgroundWidth
    yCircleBorder = yCircleBorder*playgroundHeight
    rCircleBorder = Math.sqrt(rCircleBorder*playgroundWidth*playgroundHeight)
    Qt.createQmlObject('import QtQuick 2.2;Rectangle{id: child' + childNumber + ';x:'+ xCircleBorder +'- ('+ rCircleBorder + '+' + marginCircleBorder + '/2);y: '+ yCircleBorder +'- ('+ rCircleBorder + '+' + marginCircleBorder + '/2);height: ('+ rCircleBorder + '+' + marginCircleBorder + '/2) * 2;width: ('+ rCircleBorder + '+' + marginCircleBorder + '/2) * 2;color: "#00000000";border.color: "#A9E2F3";border.width: '+ marginCircleBorder+ ';radius: ('+ rCircleBorder + '+' + marginCircleBorder +'/2);}',
                       parent,'child' + childNumber)
}

function drawNewRectangleInner(xRectangleInner, yRectangleInner, heightRectangleInner, widthRectangleInner, childNumber, playgroundWidth, playgroundHeight){
    xRectangleInner= xRectangleInner*playgroundWidth
    yRectangleInner= yRectangleInner*playgroundHeight
    widthRectangleInner= widthRectangleInner*playgroundWidth
    heightRectangleInner= heightRectangleInner*playgroundHeight

    Qt.createQmlObject(
                'import QtQuick 2.2;Rectangle{id: child' + childNumber + ';x:'+ xRectangleInner +';y: '+ yRectangleInner + ';height: '+ heightRectangleInner + ';width: '+ widthRectangleInner +';color: "yellow";border.color: "black";border.width: 1;}',
                parent,'child' + childNumber)
}

function drawNewRectangleBorder(xRectangleBorder, yRectangleBorder, heightRectangleBorder, widthRectangleBorder, marginRectangleBorder, childNumber, playgroundWidth, playgroundHeight){
    marginRectangleBorder = marginRectangleBorder*playgroundWidth
    xRectangleBorder= xRectangleBorder*playgroundWidth
    yRectangleBorder= yRectangleBorder*playgroundHeight
    widthRectangleBorder= widthRectangleBorder*playgroundWidth
    heightRectangleBorder= heightRectangleBorder*playgroundHeight

    Qt.createQmlObject(
                'import QtQuick 2.2;  \
        Item{
            Rectangle{ \
        parent: parent.parent; \
        x:' + xRectangleBorder + '; \
        y:' + yRectangleBorder + '; \
        height: '+ heightRectangleBorder +'; \
        width: '+ widthRectangleBorder +';\
        color: "#00000000";border.color: "#FFFFE5"; \
        border.width:' + marginRectangleBorder/2 + '; \
            }

            Rectangle{ \
        parent: parent.parent; \
        x:' + (xRectangleBorder - marginRectangleBorder/2)+ '; \
        y:' + (yRectangleBorder - marginRectangleBorder/2)+ '; \
        height: '+ (heightRectangleBorder + marginRectangleBorder) +'; \
        width: '+ (widthRectangleBorder + marginRectangleBorder) +';\
        color: "#00000000";border.color: "#FFFFE5"; \
        border.width:' + marginRectangleBorder/2 + '; \

            }}',
                parent,'child' + childNumber)
}

function drawNewLineDistance(x1LineDistance, y1LineDistance, x2LineDistance, y2LineDistance, childNumber, playgroundWidth, playgroundHeight){
    x1LineDistance= x1LineDistance*playgroundWidth
    y1LineDistance= y1LineDistance*playgroundHeight
    x2LineDistance= x2LineDistance*playgroundWidth
    y2LineDistance= y2LineDistance*playgroundHeight

    Qt.createQmlObject(
                'import QtQuick 2.2;Canvas{id: child' + childNumber + ';width: parent.width; height: parent.height;anchors.centerIn: parent;onPaint: {var ctx = child' + childNumber + '.getContext("2d");ctx.fillStyle="black";ctx.beginPath();ctx.moveTo(' + x1LineDistance + ',' + y1LineDistance + ');ctx.lineTo(' + x2LineDistance + ',' + y2LineDistance + ');ctx.stroke();ctx.closePath();}}',
                parent,'child' + childNumber)
}

function drawNewPointDistance(xPointDistance, yPointDistance, childNumber, playgroundWidth, playgroundHeight){
    xPointDistance= xPointDistance*playgroundWidth
    yPointDistance= yPointDistance*playgroundHeight

    Qt.createQmlObject(
                'import QtQuick 2.2;Rectangle{id: child' + childNumber + ';x:'+ xPointDistance + '-width/2;y: '+ yPointDistance + '- height/2 ;height: 5 ;width: 5 ;color: "black" ;border.color: "black";border.width:1;}',
                parent,'child' + childNumber)
}

function drawNewIrrPolygonInner(verticesIrrPolygonInner, childNumber, playgroundWidth, playgroundHeight){
    var temp = ""
    var  newVertices = []
    for(var i=0; i<verticesIrrPolygonInner.length; i++){
        newVertices[i] = Qt.point(verticesIrrPolygonInner[i].x * playgroundWidth, verticesIrrPolygonInner[i].y*playgroundHeight)
    }
    verticesIrrPolygonInner = newVertices
    for(var i = 1; i < verticesIrrPolygonInner.length; i++){
        temp += 'ctx.lineTo(' + verticesIrrPolygonInner[i]["x"] + ', ' + verticesIrrPolygonInner[i]["y"] + ');'
    }
    Qt.createQmlObject(
                'import QtQuick 2.2;Canvas{id: child' + childNumber + ';width: parent.width; height: parent.height;anchors.centerIn: parent; \
                onPaint: {var ctx = child' + childNumber + '.getContext("2d");ctx.fillStyle="black"; \
                ctx.beginPath(); \
                ctx.moveTo( ' +  verticesIrrPolygonInner[0]["x"] + ',' + verticesIrrPolygonInner[0]["y"] + ');'
                + temp +
                'ctx.lineTo( ' + verticesIrrPolygonInner[0]["x"] + ', ' + verticesIrrPolygonInner[0]["y"] + '); \
                ctx.fillStyle = "purple"; \
                ctx.fill(); \
                ctx.stroke(); \
                ctx.stroke();ctx.closePath();}}',
                parent,'child' + childNumber)
}


function drawNewRegPolygonInner(verticesRegPolygonInner, childNumber, playgroundWidth, playgroundHeight){
    var temp = ""
    var  newVertices = []
    for(var i=0; i<verticesRegPolygonInner.length; i++){
        newVertices[i] = Qt.point(verticesRegPolygonInner[i].x * playgroundWidth, verticesRegPolygonInner[i].y*playgroundHeight)
    }
    verticesRegPolygonInner = newVertices
    for(var i = 1; i < verticesRegPolygonInner.length; i++){
        temp += 'ctx.lineTo(' + verticesRegPolygonInner[i]["x"] + ', ' + verticesRegPolygonInner[i]["y"] + ');'
    }
    Qt.createQmlObject(
                'import QtQuick 2.2;Canvas{id: child' + childNumber + ';width: parent.width; height: parent.height;anchors.centerIn: parent; \
                onPaint: {var ctx = child' + childNumber + '.getContext("2d");ctx.fillStyle="black"; \
                ctx.beginPath(); \
                ctx.moveTo( ' +  verticesRegPolygonInner[0]["x"] + ',' + verticesRegPolygonInner[0]["y"] + ');'
                + temp +
                'ctx.lineTo( ' + verticesRegPolygonInner[0]["x"] + ', ' + verticesRegPolygonInner[0]["y"] + '); \
                ctx.fillStyle = "pink"; \
                ctx.fill(); \
                ctx.stroke(); \
                ctx.stroke();ctx.closePath();}}',
                parent,'child' + childNumber)
}

function drawNewZone(type, properties, childnumber, playgroundWidth, playgroundHeight){
    if(parent == null){
        console.log("You forgot to set a visual parent with setVisualParent or your parent is null")

    }else{

        console.log("AFTER SIGNAL: Type " + type + " Properties " + properties + " playground width " + playgroundWidth + " playgroundHeight " + playgroundHeight)

        for (var prop in properties) {
            console.log("Object item:", prop, "=", properties[prop])
        }

        switch(type){
        case CelluloZoneTypes.CIRCLEINNER:
            drawNewCircleInner(properties["r"], properties["x"], properties["y"], childnumber, playgroundWidth, playgroundHeight)
            break;
        case CelluloZoneTypes.CIRCLEBORDER:
            drawNewCircleBorder(properties["r"], properties["x"], properties["y"], properties["marginThickeness"], childnumber, playgroundWidth, playgroundHeight)
            break;
            //        case CelluloZone::CIRCLEDISTANCE:
            //            break;
        case CelluloZoneTypes.RECTANGLEINNER:
            drawNewRectangleInner(properties["x"], properties["y"], properties["height"], properties["width"], childnumber, playgroundWidth, playgroundHeight)
            break;
        case CelluloZoneTypes.RECTANGLEBORDER:
            drawNewRectangleBorder(properties["x"], properties["y"], properties["height"], properties["width"], properties["marginThickeness"], childnumber, playgroundWidth, playgroundHeight)
            break;
            //        case CelluloZone::RECTANGLEDISTANCE:
            //            break;
            //        case CelluloZone::LINEBORDER:
            //            break;
        case CelluloZoneTypes.LINEDISTANCE:
            drawNewLineDistance(properties["x1"], properties["y1"], properties["x2"], properties["y2"], childnumber, playgroundWidth, playgroundHeight)
            break;
        case CelluloZoneTypes.POINTDISTANCE:
            drawNewPointDistance(properties["x"], properties["y"], childnumber, playgroundWidth, playgroundHeight)
            break;
        case CelluloZoneTypes.RPOLYGONINNER:
            drawNewRegPolygonInner(properties["vertices"], childnumber, playgroundWidth, playgroundHeight)

            break;
            //        case CelluloZone::RPOLYGONBORDER:
            //            break;
            //        case CelluloZone::RPOLYGONDISTANCE:
            //            break;
        case CelluloZoneTypes.IRPOLYGONINNER:
            drawNewIrrPolygonInner(properties["vertices"], childnumber, playgroundWidth, playgroundHeight)
            break;
            //        case CelluloZone::IRPOLYGONBORDER:
            //            break;
            //        case CelluloZone::IRPOLYGONDISTANCE:
            //            break;
        default:
            console.debug("Forgot to handle an enum case");
            break;
        }
    }
}
