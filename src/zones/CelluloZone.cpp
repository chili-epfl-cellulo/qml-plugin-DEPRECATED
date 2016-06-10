#include "CelluloZone.h"
#include "src/CelluloBluetooth.h"

CelluloZone::CelluloZone(QQuickItem* parent) :
    QQuickItem(parent)
{
    marginThickeness = 0;
    //TODO active state by default?
    active = true;
}


void CelluloZone::calculateOnPoseChanged(){
    CelluloBluetooth *cellulo = qobject_cast<CelluloBluetooth *>(QObject::sender());
    if(cellulo && active)
    {
        changeInCellulosCalculate(cellulo->getMacAddr(),this->calculate(cellulo->getX()*0.508, cellulo->getY()*0.508, cellulo->getTheta()));
    }
}


void CelluloZone::changeInCellulosCalculate(const QString& key, float value){
    // only emit value changed if the value has actually changed
    QHash<QString, float>::iterator it = cellulosCalculate.find(key);
    if(it != cellulosCalculate.end()){
        //qDebug() << "Robot already in hashmap " << key << " " << value;
        if(it.value() != value){
            qDebug() << "New value for robot" << key << " " << value;
            cellulosCalculate[key] = value;
            emit calculateCelluloChanged(key, value);
        }
    } else {
        qDebug() << "New robot in hashmap " << key << " " << value;
        cellulosCalculate.insert(key, value);
        emit calculateCelluloChanged(key, value);
    }
}
