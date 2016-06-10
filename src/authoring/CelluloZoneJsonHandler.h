#ifndef CELLULOZONEJSONHANDLER_H
#define CELLULOZONEJSONHANDLER_H

#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "src/zones/CelluloZone.h"
#include "src/zones/CelluloZoneCircle.h"
#include "src/zones/CelluloZoneRectangle.h"
#include "src/zones/CelluloZoneLine.h"
#include "src/zones/CelluloZonePoint.h"
#include "src/zones/CelluloZonePolygon.h"

class CelluloZoneJsonHandler : public QQuickItem {
    Q_OBJECT

public:

    Q_INVOKABLE static bool saveQMLZones(QList<QVariant> qmlZones, QString path, const float mmPgdWidth, const float mmPgdHeight);
    Q_INVOKABLE static QMap<QString, QVariant> loadQMLZones(QString path);

    bool static saveZones(QList<CelluloZone*> zones, QString path, const float mmPgdWidth, const float mmPgdHeight);
    QMap<QString, QVariant> static loadZones(QString path);

    double static getRealPlaygroundWidthFromVariant(QMap<QString, QVariant> map);
    double static getRealPlaygroundHeightFromVariant(QMap<QString, QVariant> map);
    QList<CelluloZone*> static getZonesFromVariant(QMap<QString, QVariant> map);

private:
    //It provides read() and write() functions to serialise its member variables.
    bool static write(QJsonObject &json, QList<CelluloZone*> zones, const float mmPgdWidth, const float mmPgdHeight);
    QMap<QString, QVariant> static read(const QJsonObject &json);


};


#endif // CELLULOZONEJSONHANDLER_H
