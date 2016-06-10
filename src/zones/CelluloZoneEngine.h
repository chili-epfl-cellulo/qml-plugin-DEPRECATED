#ifndef CELLULOZONEENGINE_H
#define CELLULOZONEENGINE_H

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include "CelluloZoneEngine.h"
#include "CelluloZoneCircle.h"
#include "CelluloZoneRectangle.h"
#include "CelluloZoneLine.h"
#include "CelluloZonePoint.h"
#include "CelluloZonePolygon.h"
#include "src/authoring/CelluloZoneJsonHandler.h"

#include "src/CelluloBluetooth.h"


class CelluloZoneEngine : public QQuickItem{
    Q_OBJECT
    Q_PROPERTY(float vRplaygroundWidth WRITE setVRPlaygroundWidth READ getVRPlaygroundWidth NOTIFY VRplaygroundWidthChanged)
    Q_PROPERTY(float vRplaygroundHeight WRITE setVRPlaygroundHeight READ getVRPlaygroundHeight NOTIFY VRplaygroundHeightChanged)
    Q_PROPERTY(float realPlaygroundWidth WRITE setRealPlaygroundWidth READ getRealPlaygroundWidth  NOTIFY realPlaygroundWidthChanged)
    Q_PROPERTY(float realPlaygroundHeight WRITE setRealPlaygroundHeight READ getRealPlaygroundHeight  NOTIFY realPlaygroundHeightChanged)


public:

    /**
     * @brief Creates a new Cellulo zone engine
     *
     * @param parent Parent of this QML item
     */
    explicit CelluloZoneEngine(QQuickItem* parent = 0);

    /**
     * @brief Destroys this Cellulo robot communicator
     */
    virtual ~CelluloZoneEngine();

    /**
     * @brief Get the width size of the virtual playground
     *
     * @return Width size of the virtual playground
     */
    float getVRPlaygroundWidth() { return vRplaygroundWidth; }
    /**
     * @brief Set the width size of the virtual playground
     *
     * @param newVRPlaygroundWidth New width size of the virtual playground
     */
    void setVRPlaygroundWidth(float newVRPlaygroundWidth){
        if(newVRPlaygroundWidth!=vRplaygroundWidth){
            vRplaygroundWidth = newVRPlaygroundWidth;
            emit(VRplaygroundWidthChanged());
        }
    }

    /**
     * @brief Get the height size of the virtual playground
     *
     * @return Heigth size of the virtual playground
     */
    float getVRPlaygroundHeight() { return vRplaygroundHeight; }
    /**
     * @brief Set the height size of the virtual playground
     *
     * @param newVRPlaygroundHeight New height size of the virtual playground
     */
    void setVRPlaygroundHeight(float newVRPlaygroundHeight){
        if(newVRPlaygroundHeight!=vRplaygroundHeight){
            vRplaygroundHeight = newVRPlaygroundHeight;
            emit(VRplaygroundHeightChanged());
        }
    }
    /**
     * @brief Get the width size of the real playground
     *
     * @return Width size of the real playground
     */
    float getRealPlaygroundWidth() { return realPlaygroundWidth; }
    /**
     * @brief Set the width size of the real playground
     *
     * @param newRealPlaygroundWidth New width size of the real playground
     */
    void setRealPlaygroundWidth(float newRealPlaygroundWidth){
        if(newRealPlaygroundWidth!=realPlaygroundWidth){
            realPlaygroundWidth = newRealPlaygroundWidth;
            emit(realPlaygroundWidthChanged());
        }
    }
    /**
     * @brief Get the height size of the real playground
     *
     * @return Heigth size of the real playground
     */
    float getRealPlaygroundHeight() { return realPlaygroundHeight; }
    /**
     * @brief Set the height size of the real playground
     *
     * @param newRealPlaygroundHeight New height size of the real playground
     */
    void setRealPlaygroundHeight(float newRealPlaygroundHeight){
        if(newRealPlaygroundHeight!=realPlaygroundHeight){
            realPlaygroundHeight = newRealPlaygroundHeight;
            emit(realPlaygroundHeightChanged());
        }
    }

    Q_INVOKABLE QVariantList calculateForAllChildren(float xRobot, float yRobot, float thetaRobot);
    /**
     * @brief Get all zones handled by this engine and save them in a json file
     *
     * @param path path of the json file containing zones to be loaded
     *
     * @return  true if operation was successful, 0 otherwise
     */
    Q_INVOKABLE bool getAllZonesAndSaveThem(QString path);
    /**
     * @brief Add new zone from qml world
     *
     * @param type type of the zone
     * @param properties properties of the zone
     *
     * @return true if operation was successful, 0 otherwise
     */
    Q_INVOKABLE bool addNewZoneFromQML(int typeStringVersion, QVariantMap properties);
    /**
     * @brief Add new zones from json file
     *
     * @param jsonPath Path of the json file containing zones to be loaded
     *
     * @return True if operation was successful, 0 otherwise
     */
    Q_INVOKABLE bool addNewZoneFromJSON(QString jsonPath);
    /**
     * @brief Get all the names of the zone handled by this engine
     *
     * @return names of the zone handled by this engine
     */
    Q_INVOKABLE QStringList getAllZoneNames();
    /**
     * @brief Get zone handled by this engine having the corresponding name
     *
     * @param name Lookup name for the zone
     *
     * @return First zone handled by this engine to have this name or empty qvariant if there was a problem
     */
    Q_INVOKABLE QVariant getZoneFromName(QString name);



private slots:
    /**
     * @brief Called when the engine is set up in order to find initial static robots and zone and bind them together
     */
    void traverseTreeForCelluloRobotFinding();

signals:
    /**
     * @brief Emitted when new Zone has been created
     */
    void newZoneCreatedReadyForVisualization(int type, QVariantMap properties, int childNumber, float vRplaygroundWidth, float vRplaygroundHeight);
    /**
     * @brief Emitted when the width size of the virtual playground changes
     */
    void VRplaygroundWidthChanged();
    /**
     * @brief Emitted when the height size of the virtual playground changes
     */
    void VRplaygroundHeightChanged();
    /**
     * @brief Emitted when the width size of the real playground changes
     */
    void realPlaygroundWidthChanged();
    /**
     * @brief Emitted when the height size of the real playground changes
     */
    void realPlaygroundHeightChanged();

private:
    float vRplaygroundWidth;                            ///< Width size of the virtual playground
    float vRplaygroundHeight;                           ///< Height size of the virtual playground
    float realPlaygroundWidth;                          ///< Width size of the real playground
    float realPlaygroundHeight;                         ///< Height size of the virtual playground
    QList<CelluloBluetooth*> celluloBluetoothRobots;    ///< QML Static cellulo robots known by the engine

    /**
     * @brief Bind zone's selected signals and slots with all known robots's selected signal and slots
     *
     * @param zone Zone to be binded
     */
    void bindRobotsAndZone(CelluloZone* zone);
    /**
     * @brief Set engine as parent of the zone and bind it with existing robots
     *
     * @param zone Zone to be child of the engine and binded with existing robots
     */
    void setParentToZone(CelluloZone* zone);

};



#endif // CELLULOZONEENGINE_H
