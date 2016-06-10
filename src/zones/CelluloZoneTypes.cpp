#include "CelluloZoneTypes.h"

QString CelluloZoneTypes::typeToString(const CelluloZoneTypes::ZoneType &type){
    const QMetaObject &mo = CelluloZoneTypes::staticMetaObject;
    int index = mo.indexOfEnumerator("ZoneType");
    QMetaEnum metaEnum = mo.enumerator(index);
    return metaEnum.valueToKey(type);
}



CelluloZoneTypes::ZoneType CelluloZoneTypes::typeFromString(const QString &typeName){
    const QMetaObject &mo = CelluloZoneTypes::staticMetaObject;
    int index = mo.indexOfEnumerator("ZoneType");
    QMetaEnum metaEnum = mo.enumerator(index);
    int value = metaEnum.keyToValue(qPrintable(typeName));
    return static_cast<CelluloZoneTypes::ZoneType>(value);
}
