#include <QQmlProperty>
#include "CelluloZoneEngine.h"
#include <QSignalMapper>

CelluloZoneEngine::CelluloZoneEngine(QQuickItem* parent) :
    QQuickItem(parent)
{
    vRplaygroundWidth = -1;
    vRplaygroundHeight = -1;
    realPlaygroundWidth = -1;
    realPlaygroundHeight = -1;
    //Fires lookup for original zones and robots as soon as the event loop is up and running and there are no more startup events pending.
    QTimer::singleShot(0, this, SLOT(traverseTreeForCelluloRobotFinding()));
}

CelluloZoneEngine::~CelluloZoneEngine(){ }

void CelluloZoneEngine::traverseTreeForCelluloRobotFinding(){

    //find original robots
    celluloBluetoothRobots = this->parent()->findChildren<CelluloBluetooth *>();

    //find original zones
    QList<CelluloZone*> celluloZones = this->parent()->findChildren<CelluloZone *>();

    //bind original robots and zones
    foreach(CelluloZone *zone, celluloZones) {
        bindRobotsAndZone(zone);
    }


}

void CelluloZoneEngine::bindRobotsAndZone(CelluloZone* zone){
    for (int i = 0; i < celluloBluetoothRobots.length(); ++i) {

        CelluloBluetooth *robot = celluloBluetoothRobots.at(i);
        qDebug() << "Binding for robot " << robot->getMacAddr() << " with zone "  << zone->getName() << " of type " << zone->metaObject()->className();

        connect(robot, SIGNAL(poseChanged()), zone, SLOT(calculateOnPoseChanged()));
        connect(zone, SIGNAL(calculateCelluloChanged(const QString&, float)), robot, SLOT(actOnZoneCalculateChanged(const QString&, float)));

    }
    emit(newZoneCreatedReadyForVisualization(zone->getType(), zone->getRatioProperties(realPlaygroundWidth, realPlaygroundHeight), this->children().size(), vRplaygroundWidth, vRplaygroundHeight));
}


bool CelluloZoneEngine::addNewZoneFromQML(int type, QVariantMap properties){
    QQuickItem *childrenItem;
    switch (type) {
    case CelluloZoneTypes::CIRCLEINNER:
        childrenItem = new CelluloZoneCircleInner();
        break;
    case CelluloZoneTypes::CIRCLEBORDER:
        childrenItem = new CelluloZoneCircleBorder();
        break;
    case CelluloZoneTypes::CIRCLEDISTANCE:
        childrenItem = new CelluloZoneCircleDistance();
        break;
    case CelluloZoneTypes::RECTANGLEINNER:
        childrenItem = new CelluloZoneRectangleInner();
        break;
    case CelluloZoneTypes::RECTANGLEBORDER:
        childrenItem = new CelluloZoneRectangleBorder();
        break;
    case CelluloZoneTypes::LINEDISTANCE:
        childrenItem = new CelluloZoneLineDistance();
        break;
    case CelluloZoneTypes::POINTDISTANCE:
        childrenItem = new CelluloZonePointDistance();
        break;
    case CelluloZoneTypes::RPOLYGONINNER:
        childrenItem = new CelluloZoneRegularPolygonInner();
        break;
    case CelluloZoneTypes::IRPOLYGONINNER:
        childrenItem = new CelluloZoneIrregularPolygonInner();
        break;
    default:
        qDebug() << "Forgot to handle an enum case";
        return false;
        break;
    }

    CelluloZone* zone = qobject_cast<CelluloZone *>(childrenItem);

    if(zone)
    {
        Q_FOREACH(QString property, properties.keys()){
            if(zone->getRatioProperties(realPlaygroundWidth,realPlaygroundHeight).contains(property.toStdString().c_str())){
                zone->setProperty(property.toStdString().c_str(), properties[property]);
            } else {
                qDebug() << "Problem when setting property to the zone, property '" << property.toStdString().c_str() << "' not valid for this zone";
                return false;
            }
        }
        zone->setProperty("realPlaygroundWidth", realPlaygroundWidth);
        zone->setProperty("realPlaygroundHeight", realPlaygroundHeight);
        setParentToZone(zone);
        return true;

    } else{
        qDebug() << "Problem when casting object to CelluloZone";
        return false;
    }
}

//TODO renames addNewZoneS + jsonPath -> path
bool CelluloZoneEngine::addNewZoneFromJSON(QString jsonPath){
    QMap<QString, QVariant> result = CelluloZoneJsonHandler::loadZones(jsonPath);
    QList<CelluloZone*> zonesFromJSON;
    QList<QVariant> list = result["zones"].toList();
    for(int i=0; i<list.length(); i++){
        CelluloZone* zone = qvariant_cast<CelluloZone*>(list[i]);
        zonesFromJSON.append(zone);
    }
    qDebug() << result["zones"] << result["realPlaygroundWidth"] << result["realPlaygroundHeight"];
    if(zonesFromJSON.length()>0){
        foreach (CelluloZone* zone, zonesFromJSON) {
            if(zone){
                setParentToZone(zone);
                double jsonRealPlaygroundWidth = qvariant_cast<double>(result["realPlaygroundWidth"]);
                if(jsonRealPlaygroundWidth){
                    if(realPlaygroundWidth != -1){
                        qDebug() << "Real width value you provided and the one from the JSON file differ. Taking JSON file value";
                    }
                    realPlaygroundWidth = jsonRealPlaygroundWidth;
                    emit realPlaygroundWidthChanged();
                } else {
                    qDebug() << "Problem no width of playground given by jsonHandler";
                    return false;
                }
                double jsonRealPlaygroundHeight = qvariant_cast<double>(result["realPlaygroundHeight"]);
                if(jsonRealPlaygroundHeight){
                    if(realPlaygroundHeight != -1){
                        qDebug() << "Real height value you provided and the one from the JSON file differ. Taking JSON file value";
                    }
                    realPlaygroundHeight = jsonRealPlaygroundHeight;
                    emit realPlaygroundHeightChanged();
                } else {
                    qDebug() << "Problem no height of playground given by jsonHandler";
                    return false;
                }
            }else {
                qDebug() << "Problem when casting zone from json representation";
                return false;
            }
        }
    } else{
        qDebug() << "Problem when loading zone with jsonHandler";
        return false;
    }
    return true;
}
void CelluloZoneEngine::setParentToZone(CelluloZone* zone){
    QQmlEngine::setObjectOwnership(zone, QQmlEngine::CppOwnership);

    zone->setParent(this);
    zone->setParentItem(this);

    bindRobotsAndZone(zone);
}

QVariantList CelluloZoneEngine::calculateForAllChildren(float xRobot, float yRobot, float thetaRobot){
    QList<CelluloZone*> list = this->findChildren<CelluloZone *>();
    QVariantList calculateResults;
    float calculateResult;
    foreach(CelluloZone *z, list) {
        calculateResult = z->calculate(xRobot, yRobot, thetaRobot);
        calculateResults.append(QVariant(calculateResult));
    }
    return calculateResults;
}


QStringList CelluloZoneEngine::getAllZoneNames(){
    QStringList zonesNames;
    QList<CelluloZone*> list = this->findChildren<CelluloZone *>();
    foreach(CelluloZone *z, list) {
        zonesNames.append(z->getName());
    }
    return zonesNames;
}


QVariant CelluloZoneEngine::getZoneFromName(QString name){
    QList<CelluloZone*> list = this->findChildren<CelluloZone *>();
    QVariant returnedZone;
    foreach(CelluloZone *z, list) {
        QString zoneName = z->getName();
        if(zoneName == name){
            switch (z->getType()) {
            case CelluloZoneTypes::CIRCLEINNER:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneCircleInner *>(z));
                break;
            case CelluloZoneTypes::CIRCLEBORDER:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneCircleBorder *>(z));
                break;
            case CelluloZoneTypes::CIRCLEDISTANCE:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneCircleDistance *>(z));
                break;
            case CelluloZoneTypes::RECTANGLEINNER:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneRectangleInner *>(z));
                break;
            case CelluloZoneTypes::RECTANGLEBORDER:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneRectangleBorder *>(z));
                break;
            case CelluloZoneTypes::LINEDISTANCE:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneLineDistance *>(z));
                break;
            case CelluloZoneTypes::POINTDISTANCE:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZonePointDistance *>(z));
                break;
            case CelluloZoneTypes::RPOLYGONINNER:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneRegularPolygonInner *>(z));
                break;
            case CelluloZoneTypes::IRPOLYGONINNER:
                returnedZone = QVariant::fromValue(dynamic_cast<CelluloZoneIrregularPolygonInner *>(z));
                break;
            default:
                qDebug() << "Forgot to handle an enum case";
                return QVariant();
                break;
            }
            return returnedZone;
        }
    }
    qDebug() << "No zone handled by this engine";
    return QVariant();
}


bool CelluloZoneEngine::getAllZonesAndSaveThem(QString path){
    QList<CelluloZone*> zones = this->parent()->findChildren<CelluloZone *>();
    return  CelluloZoneJsonHandler::saveZones(zones, path, realPlaygroundWidth, realPlaygroundHeight);
}
