#ifndef CELLULOZONELINE_H
#define CELLULOZONELINE_H

#include "CelluloZone.h"

/**
 * @brief CelluloZone Base Class for line zones
 */
class CelluloZoneLine : public CelluloZone {
    Q_OBJECT
    Q_PROPERTY(float x1 WRITE setX1 READ getX1 NOTIFY x1Changed)
    Q_PROPERTY(float y1 WRITE setY1 READ getY1 NOTIFY y1Changed)
    Q_PROPERTY(float x2 WRITE setX2 READ getX2 NOTIFY x2Changed)
    Q_PROPERTY(float y2 WRITE setY2 READ getY2 NOTIFY y2Changed)

public:
    /**
     * @brief Creates a new Cellulo line zone
     */
    CelluloZoneLine();

    /**
     * @brief Gets the x position of the first line's point
     * @return X position of the first line's point
     */
    float getX1() { return x1; }
    void setX1(float newx1) {
        if(newx1!=x1){
            x1 = newx1;
            emit(x1Changed());
        }
    }

    /**
     * @brief Gets the y position of the first line's point
     * @return Y position of the first line's point
     */
    float getY1() { return y1; }
    void setY1(float newY1) {
        if(newY1!=y1){
            y1 = newY1;
            emit(y1Changed());
        }
    }

    /**
     * @brief Gets the x position of the second line's point
     * @return X position of the second line's point
     */
    float getX2() { return x2; }
    void setX2(float newx2) {
        if(newx2!=x2){
            x2 = newx2;
            emit(x2Changed());
        }
    }

    /**
     * @brief Gets the y position of the second line's point
     * @return Y position of the second line's point
     */
    float getY2() { return y2; }
    void setY2(float newY2) {
        if(newY2!=y2){
            y2 = newY2;
            emit(y2Changed());
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
     * @brief Calculate the computation of this line zone according to the robot's pose
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
    float x1;                               ///< X position of the first line's point
    float y1;                               ///< Y position of the first line's point
    float x2;                               ///< X position of the second line's point
    float y2;                               ///< Y position of the second line's point

signals:

    /**
     * @brief Emitted when the X position of the first line's point changes
     */
    void x1Changed();
    /**
     * @brief Emitted when the Y position of the first line's point changes
     */
    void y1Changed();
    /**
     * @brief Emitted when the X position of the second line's point changes
     */
    void x2Changed();
    /**
     * @brief Emitted when the Y position of the second line's point changes
     */
    void y2Changed();

};

/**
 * @brief CelluloZone Specific Class for line zones distance determination
 */
class CelluloZoneLineDistance : public CelluloZoneLine {
    Q_OBJECT

public:
    CelluloZoneLineDistance();

    /**
     * @brief Calculate the orthogonal distance between the robot position and this line zone
     * @return the orthogonal distance between the robot position and this line zone
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);

};

#endif // CELLULOZONELINE_H
