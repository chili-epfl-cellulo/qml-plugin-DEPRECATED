#ifndef CELLULOZONECIRCLE_H
#define CELLULOZONECIRCLE_H

#include "CelluloZone.h"

/**
 * @brief CelluloZone Base Class for circular zones
 */
class CelluloZoneCircle : public CelluloZone {
    Q_OBJECT
    Q_PROPERTY(float x WRITE setX READ getX NOTIFY xChanged)
    Q_PROPERTY(float y WRITE setY READ getY NOTIFY yChanged)
    Q_PROPERTY(float r WRITE setR READ getR NOTIFY rChanged)



public:
    /**
     * @brief Creates a new Cellulo circular zone
     */
    CelluloZoneCircle();

    /**
     * @brief Gets the x position of circle's center
     * @return X position of circle's center
     */
    float getX() { return x; }
    void setX(float newX) {
        if(newX!=x){
            x = newX;
            emit(xChanged());
        }
    }

    /**
     * @brief Gets the y position of circle's center
     * @return Y position of circle's center
     */
    float getY() { return y; }
    void setY(float newY) {
        if(newY!=y){
            y = newY;
            emit(yChanged());
        }
    }

    /**
     * @brief Gets the radius of the circle
     * @return R the radius of the circle
     */
    float getR() { return r; }
    void setR( float newR) {
        if(newR!=r){
            r = newR;
            emit(rChanged());
        }
    }

    /**
     * @brief get the properties of this zone in rationalized form
     * @param realPlaygroundWidth real playground width
     * @param realPlaygroundHeight real playground height
     * @return map of rationalized properties
     */
    QVariantMap getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight);

    /**
     * @brief Calculate the computation of this circular zone according to the robot's pose
     * @return Computation's result for this zone and this robot's pose
     */
    Q_INVOKABLE virtual float calculate(float xRobot, float yRobot, float thetaRobot) = 0;

    /**
     * @brief Write the zone infos to the given json Object
     * @param QJsonObject json object to be written
     */
    void write(QJsonObject &json);


    /**
     * @brief Read the zone infos from the given json Object
     * @param json json object to be read
     */
    void read(const QJsonObject &json);




protected:
    float x;                                ///< x position of circle's center
    float y;                                ///< y position of circle's center
    float r;                                ///< r the radius of the circle

signals:

    /**
     * @brief Emitted when the x position of circular zone's center changes
     */
    void xChanged();
    /**
     * @brief Emitted when the y position of circular zone's center changes
     */
    void yChanged();
    /**
     * @brief Emitted when r the radius of the circular zone changes
     */
    void rChanged();


};

/**
 * @brief CelluloZone Specific Class for circular zones inner determination
 */
class CelluloZoneCircleInner : public CelluloZoneCircle {
    Q_OBJECT

public:
    CelluloZoneCircleInner();

    /**
     * @brief Calculate whether the robot lies inside this circular zone
     * @return 1 if the robot is inside this circular zone 0 otherwise
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);
};

/**
 * @brief CelluloZone Specific Class for circular zones border determination
 */
class CelluloZoneCircleBorder : public CelluloZoneCircle {
    Q_OBJECT

public:
    CelluloZoneCircleBorder();


    /**
     * @brief Calculate whether the robot lies on the border of this circular zone (given the zone's margin)
     * @return 1 if the robot is on the border of this circular zone (given the zone's margin) 0 otherwise
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);

};

/**
 * @brief CelluloZone Specific Class for circular zones distance determination
 */
class CelluloZoneCircleDistance : public CelluloZoneCircle {
    Q_OBJECT

public:
    CelluloZoneCircleDistance();


    /**
     * @brief Calculate the distance between the robot pose and the border of this circular zone
     * @return the distance between the robot pose and the border of this circular zone
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);

};

#endif // CELLULOZONECIRCLE_H
