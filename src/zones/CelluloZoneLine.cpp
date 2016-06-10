#include "CelluloZoneLine.h"

CelluloZoneLine::CelluloZoneLine() :
    CelluloZone()
{
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

QVariantMap CelluloZoneLine::getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight){
    QVariantMap properties;
    properties["name"] = QVariant(name);
    properties["marginThickeness"] = QVariant(marginThickeness/realPlaygroundWidth);
    properties["x1"] = QVariant(x1/realPlaygroundWidth);
    properties["y1"] = QVariant(y1/realPlaygroundHeight);
    properties["x2"] = QVariant(x2/realPlaygroundWidth);
    properties["y2"] = QVariant(y2/realPlaygroundHeight);
    return properties;
}

void CelluloZoneLine::write(QJsonObject &json){
    json["type"] = CelluloZoneTypes::typeToString(type);
    json["marginThickeness"] = marginThickeness;
    json["name"] = name;
    json["x1"] = x1;
    json["y1"] = y1;
    json["x2"] = x2;
    json["y2"] = y2;
}

void CelluloZoneLine::read(const QJsonObject &json){
    type = CelluloZoneTypes::typeFromString(json["type"].toString());
    marginThickeness = json["marginThickeness"].toDouble();
    name = json["name"].toString();
    x1 = json["x1"].toDouble();
    y1 = json["y1"].toDouble();
    x2 = json["x2"].toDouble();
    y2 = json["y2"].toDouble();
}

CelluloZoneLineDistance::CelluloZoneLineDistance() :
    CelluloZoneLine()
{
    type = CelluloZoneTypes::LINEDISTANCE;
}

float CelluloZoneLineDistance::calculate(float xRobot, float yRobot, float thetaRobot){
    float numerator = (y2-y1)*xRobot - (x2-x1)*yRobot + x2*y1 - y2*x1;
    float denumerator = sqrt((y2-y1)*(y2-y1) + (x2 -x1)*(x2 -x1));
    return fabs(numerator/denumerator);
}
