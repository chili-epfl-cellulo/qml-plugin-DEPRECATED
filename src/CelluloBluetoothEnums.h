/*
 * Copyright (C) 2016 EPFL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

/**
 * @file CelluloEnumDecl.h
 * @brief Header for additional useful Q_ENUM declarations
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2016-06-07
 */

#include <QObject>
#include <QMetaEnum>

class CelluloBluetoothEnums : public QObject{
/* *INDENT-OFF* */
Q_OBJECT
/* *INDENT-ON* */

public:

    CelluloBluetoothEnums(QObject* parent = 0) : QObject(parent){}

    ~CelluloBluetoothEnums(){}

    static QObject* CelluloBluetoothEnumsSingletonTypeProvider(QQmlEngine* qmlEngine, QJSEngine* jsEngine){
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine)

        return new CelluloBluetoothEnums();
    }

    enum Deneme {
        Deneme0 = 0,
        Deneme1 = 1,
        DenemeNumElements
    };
    Q_ENUM(Deneme)
};

/**
 * Adds the following methods to a Q_OBJECT given an enum X that starts with 0 and has
 * ENumElements as its last element:
 *
 *      QString EEnumString(E value): Returns name of enum value
 *      QVariantList EEnumStrings(): Returns list of all names of enum
 *
 * @param E Enum with above properties
 */
#define CELLULO_ENUM_DECL(E) \
    QMetaEnum E ## _MetaEnum = QMetaEnum::fromType<E>(); \
    Q_INVOKABLE QString E ## EnumString(E value){ \
        return E ## _MetaEnum.valueToKey(value); \
    } \
    Q_INVOKABLE QVariantList E ## EnumStrings(){ \
        QVariantList list; \
        for(int i=0; i<E ## NumElements; i++) \
            list << E ## EnumString((E)i); \
        return list; \
    }
