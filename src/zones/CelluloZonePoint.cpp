#include "CelluloZonePoint.h"

CelluloZonePoint::CelluloZonePoint() :
    CelluloZone()
{
    x = 0;
    y = 0;
}

QVariantMap CelluloZonePoint::getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight){
    QVariantMap properties;
    properties["name"] = QVariant(name);
    properties["marginThickeness"] = QVariant(marginThickeness/realPlaygroundWidth);
    properties["x"] = QVariant(x/realPlaygroundWidth);
    properties["y"] = QVariant(y/realPlaygroundHeight);
    return properties;
}

void CelluloZonePoint::write(QJsonObject &json){
    json["type"] = CelluloZoneTypes::typeToString(type);
    json["marginThickeness"] = marginThickeness;
    json["name"] = name;
    json["x"] = x;
    json["y"] = y;
}

void CelluloZonePoint::read(const QJsonObject &json){
    type = CelluloZoneTypes::typeFromString(json["type"].toString());
    marginThickeness = json["marginThickeness"].toDouble();
    name = json["name"].toString();
    x = json["x"].toDouble();
    y = json["y"].toDouble();
}

CelluloZonePointDistance::CelluloZonePointDistance() :
    CelluloZonePoint()
{
    type = CelluloZoneTypes::POINTDISTANCE;
}


float CelluloZonePointDistance::calculate(float xRobot, float yRobot, float thetaRobot){
    return sqrt((xRobot-x)*(xRobot-x) + (yRobot -y)*(yRobot -y));
}
