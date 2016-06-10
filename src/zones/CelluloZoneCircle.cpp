#include "CelluloZoneCircle.h"
#include <QQmlEngine>


CelluloZoneCircle::CelluloZoneCircle() :
    CelluloZone()
{
    x = 0;
    y = 0;
    r = 0;
}

QVariantMap CelluloZoneCircle::getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight){
    QVariantMap properties;
    properties["name"] = QVariant(name);
    properties["marginThickeness"] = QVariant(marginThickeness/realPlaygroundWidth);
    properties["x"] = QVariant(x/realPlaygroundWidth);
    properties["y"] = QVariant(y/realPlaygroundHeight);
    properties["r"] = QVariant((r*r)/(realPlaygroundWidth*realPlaygroundHeight));
    return properties;
}

void CelluloZoneCircle::write(QJsonObject &json){
    json["type"] = CelluloZoneTypes::typeToString(type);
    json["marginThickeness"] = marginThickeness;
    json["name"] = name;
    json["x"] = x;
    json["y"] = y;
    json["r"] = r;
}

void CelluloZoneCircle::read(const QJsonObject &json){
    type = CelluloZoneTypes::typeFromString(json["type"].toString());
    marginThickeness = json["marginThickeness"].toDouble();
    name = json["name"].toString();
    x = json["x"].toDouble();
    y = json["y"].toDouble();
    r = json["r"].toDouble();
}


CelluloZoneCircleInner::CelluloZoneCircleInner() :
    CelluloZoneCircle()
{
    type = CelluloZoneTypes::CIRCLEINNER;
}

float CelluloZoneCircleInner::calculate(float xRobot, float yRobot, float thetaRobot){
    return ((xRobot-x)*(xRobot-x) + (yRobot -y)*(yRobot -y) <= r*r)? 1 : 0;
}

CelluloZoneCircleBorder::CelluloZoneCircleBorder() :
    CelluloZoneCircle()
{
    type = CelluloZoneTypes::CIRCLEBORDER;
}


float CelluloZoneCircleBorder::calculate(float xRobot, float yRobot, float thetaRobot){
    float temp = (xRobot-x)*(xRobot-x) + (yRobot -y)*(yRobot -y);
    float rOuter = r + marginThickeness/2;
    float rInner = r - marginThickeness/2;
    return  temp <= (rOuter*rOuter) && temp > (rInner* rInner) ? 1 : 0;
}

CelluloZoneCircleDistance::CelluloZoneCircleDistance() :
    CelluloZoneCircle()
{
    type = CelluloZoneTypes::CIRCLEDISTANCE;
}


float CelluloZoneCircleDistance::calculate(float xRobot, float yRobot, float thetaRobot){
    return sqrt((xRobot-x)*(xRobot-x) + (yRobot -y)*(yRobot -y)) - r;

}
