#include "CelluloZonePolygon.h"

CelluloZonePolygon::CelluloZonePolygon() :
    CelluloZone()
{
    //TODO Init better lists if I put them as pointers
    maxX = std::numeric_limits<qreal>::min();
    maxY = std::numeric_limits<qreal>::min();
    minX = std::numeric_limits<qreal>::max();
    minY = std::numeric_limits<qreal>::max();
}

void CelluloZonePolygon::setMaxMinOuterRectangle(const QList<QPointF> &pointsQt, qreal *minX, qreal *maxX, qreal *minY, qreal *maxY){
    for (int i = 0; i < pointsQt.size(); ++i) {
        if(pointsQt.at(i).x() < *minX){
            *minX = pointsQt.at(i).x();
        }
        if(pointsQt.at(i).x() > *maxX){
            *maxX = pointsQt.at(i).x();
        }
        if(pointsQt.at(i).y() < *minY){
            *minY = pointsQt.at(i).y();
        }
        if(pointsQt.at(i).y() > *maxY){
            *maxY = pointsQt.at(i).y();
        }
    }
}

float CelluloZonePolygon::pointInPoly(float xRobot, float yRobot){
    //taken from: https://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    if(!(xRobot>maxX || xRobot<minX || yRobot>maxY || yRobot<minY)){
        int i, j, c = 0;
        for (i = 0, j = pointsQt.size()-1; i < pointsQt.size(); j = i++) {
            if ( ((pointsQt.at(i).y()>yRobot) != (pointsQt.at(j).y()>yRobot)) &&
                 (xRobot < (pointsQt.at(j).x()-pointsQt.at(i).x()) * (yRobot-pointsQt.at(i).y()) / (pointsQt.at(j).y()-pointsQt.at(i).y()) + pointsQt.at(i).x()))
                c = !c;
        }
        return c? 1:0;
    } else {
        return 0;
    }

}

void CelluloZonePolygon::setPointsQt(const QList<QPointF> &newPointsQt){
    if(newPointsQt!=pointsQt){
        pointsQt = newPointsQt;
        setMaxMinOuterRectangle(pointsQt, &minX, &maxX, &minY, &maxY);
    }
    //qDebug() << "minX: " << minX << "maxX: " << maxX << "minY: " << minY << "maxY: " << maxY ;
}

QVariantMap CelluloZoneIrregularPolygon::getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight){
    QVariantMap properties;
    properties["name"] = QVariant(name);
    properties["marginThickeness"] = QVariant(marginThickeness/realPlaygroundWidth);
    QList<QVariant> ratioVertices;
    foreach(QPointF point, convertQVariantToQPointF()) {
        QPointF ratioPoint = QPointF(point.x()/realPlaygroundWidth, point.y()/realPlaygroundHeight);
        ratioVertices.append(QVariant(ratioPoint));
    }
    properties["vertices"] = ratioVertices;
    return properties;
}

void CelluloZoneIrregularPolygon::write(QJsonObject &json){
    json["type"] = CelluloZoneTypes::typeToString(type);
    json["marginThickeness"] = marginThickeness;
    json["name"] = name;
    QJsonObject obj;
    QJsonArray verticesArray;
    foreach(QPointF point, convertQVariantToQPointF()) {
        QJsonObject pointObject;
        pointObject["x"] = point.x();
        pointObject["y"] = point.y();
        verticesArray.append(pointObject);
    }
    json["vertices"] = verticesArray;

}

void CelluloZoneIrregularPolygon::read(const QJsonObject &json){
    vertices.clear();
    type = CelluloZoneTypes::typeFromString(json["type"].toString());
    marginThickeness = json["marginThickeness"].toDouble();
    name = json["name"].toString();
    QJsonArray verticesArray = json["vertices"].toArray();
    foreach(QVariant pointObject, verticesArray.toVariantList()) {
        //TODO test canConvert
        QMap<QString, QVariant> pointMap = pointObject.toMap();
        QPointF point;
        point.setX(pointMap["x"].toDouble());
        point.setY(pointMap["y"].toDouble());
        vertices.append(point);
    }
    setPointsQt(convertQVariantToQPointF());
}

QList<QPointF> CelluloZoneIrregularPolygon::convertQVariantToQPointF(){
    QList<QPointF> newPointsQt;
    for (int i = 0; i < vertices.size(); ++i) {
        if(vertices.at(i).canConvert(QVariant::PointF)){
            if(QVariant(vertices.at(i)).convert(QVariant::PointF)){
                QPointF newPoint = vertices.at(i).toPointF();
                //qDebug() << "Point irregular: (" <<  newPoint.x() <<"," << newPoint.y() <<")";
                newPointsQt.append(newPoint);
            } else{
                qDebug() << "Problem with convert() at index " << i;
            }
        }else {
            qDebug() << "Problem with canConvert() at index " << i;
        }
    }
    return newPointsQt;
}


CelluloZoneRegularPolygon::CelluloZoneRegularPolygon() :
    CelluloZonePolygon()
{
    x = 0;
    y = 0;
    r = 0;
    numEdges = 0;
    rotAngle = 0;
}

QVariantMap CelluloZoneRegularPolygon::getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight){
    QVariantMap properties;
    properties["name"] = QVariant(name);
    properties["marginThickeness"] = QVariant(marginThickeness/realPlaygroundWidth);
    properties["numEdges"] = QVariant(numEdges);
    properties["x"] = QVariant(x/realPlaygroundHeight);
    properties["y"] = QVariant(y/realPlaygroundWidth);
    properties["r"] = QVariant((r*r)/(realPlaygroundWidth*realPlaygroundHeight));
    properties["rotAngle"] = QVariant(rotAngle);
    QList<QVariant> ratioVertices;
    foreach(QPointF point, pointsQt) {
        QPointF ratioPoint = QPointF(point.x()/realPlaygroundWidth, point.y()/realPlaygroundHeight);
        ratioVertices.append(QVariant(ratioPoint));
    }
    properties["vertices"] = ratioVertices;
    return properties;
}

void CelluloZoneRegularPolygon::write(QJsonObject &json){
    json["type"] = CelluloZoneTypes::typeToString(type);
    json["marginThickeness"] = marginThickeness;
    json["name"] = name;
    json["numEdges"] = numEdges;
    json["x"] = x;
    json["y"] = y;
    json["r"] = r;
    json["rotAngle"] = rotAngle;
}

void CelluloZoneRegularPolygon::read(const QJsonObject &json){
    type = CelluloZoneTypes::typeFromString(json["type"].toString());
    marginThickeness = json["marginThickeness"].toDouble();
    name = json["name"].toString();
    numEdges = json["numEdges"].toDouble();
    x = json["x"].toDouble();
    y = json["y"].toDouble();
    r = json["r"].toDouble();
    rotAngle = json["rotAngle"].toDouble();
    setPointsQt(createPolygonPointsFromOuterCircle());
}

QList<QPointF> CelluloZoneRegularPolygon::createPolygonPointsFromOuterCircle(){
    QList<QPointF> newPointsQt;
    if(numEdges>2){
        for (int i = 0; i < numEdges; i++) {
            //taken from: http://stackoverflow.com/questions/3436453/calculate-coordinates-of-a-regular-polygons-vertices
            QPointF newPoint = QPointF(x + r * cos(2 * M_PI * i / numEdges + rotAngle), y + r * sin(2 * M_PI * i / numEdges + rotAngle));
            qDebug() << "Point regular: (" <<  newPoint.x() <<"," << newPoint.y() <<")";
            newPointsQt.append(newPoint);
        }
    } else {
        qDebug() << "n must be >2";
    }
    return newPointsQt;
}

CelluloZoneIrregularPolygonInner::CelluloZoneIrregularPolygonInner() :
    CelluloZoneIrregularPolygon()
{
    type = CelluloZoneTypes::IRPOLYGONINNER;
}

float CelluloZoneIrregularPolygonInner::calculate(float xRobot, float yRobot, float thetaRobot){
    return pointInPoly(xRobot, yRobot);
}

CelluloZoneRegularPolygonInner::CelluloZoneRegularPolygonInner() :
    CelluloZoneRegularPolygon()
{
    type = CelluloZoneTypes::RPOLYGONINNER;
}


float CelluloZoneRegularPolygonInner::calculate(float xRobot, float yRobot, float thetaRobot){
    return pointInPoly(xRobot, yRobot);
}
