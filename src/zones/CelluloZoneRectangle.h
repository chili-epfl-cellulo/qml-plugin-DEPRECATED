#ifndef CELLULOZONERECTANGLE_H
#define CELLULOZONERECTANGLE_H

#include "CelluloZone.h"

/**
 * @brief CelluloZone Base Class for rectangular zones
 */
class CelluloZoneRectangle : public CelluloZone {
    Q_OBJECT
    Q_PROPERTY(float x WRITE setX READ getX NOTIFY xChanged)
    Q_PROPERTY(float y WRITE setY READ getY NOTIFY yChanged)
    Q_PROPERTY(float height WRITE setHeight READ getHeight NOTIFY heightChanged)
    Q_PROPERTY(float width WRITE setWidth READ getWidth NOTIFY widthChanged)




public:
    /**
     * @brief Creates a new Cellulo rectangular zone
     */
    CelluloZoneRectangle();

    /**
     * @brief get the properties of this zone in rationalized form
     * @param realPlaygroundWidth real playground width
     * @param realPlaygroundHeight real playground height
     * @return map of rationalized properties
     */
    QVariantMap getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight);

    /**
     * @brief Gets the x position of upper left corner of the rectangle
     * @return X position of upper left corner of the rectangle
     */
    float getX() { return x; }
    void setX(float newX) {
        if(newX!=x){
            x = newX;
            emit(xChanged());
        }
    }

    /**
     * @brief Gets the y position of upper left corner of the rectangle
     * @return Y position of upper left corner of the rectangle
     */
    float getY() { return y; }
    void setY(float newY) {
        if(newY!=y){
            y = newY;
            emit(yChanged());
        }
    }

    /**
     * @brief Gets the height of the rectangle
     * @return Height of the rectangle
     */
    float getHeight() { return height; }
    void setHeight(float newHeight) {
        if(newHeight!=height){
            height = newHeight;
            emit(heightChanged());
        }
    }

    /**
     * @brief Gets the width of the rectangle
     * @return Width of the rectangle
     */
    float getWidth() { return width; }
    void setWidth(float newWidth) {
        if(newWidth!=width){
            width = newWidth;
            emit(widthChanged());
        }
    }

    /**
     * @brief Calculate the computation of this rectangular zone according to the robot's pose
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
    float x;                                ///< x position of the upper left corner of the rectangle
    float y;                                ///< y position of the upper left corner of the rectangle
    float height;                           ///< height of the rectangle
    float width;                            ///< width of the rectangle

signals:

    /**
     * @brief Emitted when the x position of the upper left corner of the rectangular zone's center changes
     */
    void xChanged();
    /**
     * @brief Emitted when the y postion of the upper left corner of the rectangular zone's center changes
     */
    void yChanged();
    /**
     * @brief Emitted when the height of the rectangular zone changes
     */
    void heightChanged();

    /**
     * @brief Emitted when the height of the rectangular zone changes
     */
    void widthChanged();


};

/**
 * @brief CelluloZone Specific Class for rectangular zones inner determination
 */
class CelluloZoneRectangleInner : public CelluloZoneRectangle {
    Q_OBJECT

public:
    CelluloZoneRectangleInner();

    /**
     * @brief Calculate whether the robot lies inside this rectangular zone
     * @return 1 if the robot is inside this rectangular zone 0 otherwise
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);

};

/**
 * @brief CelluloZone Specific Class for rectangular zones border determination
 */
class CelluloZoneRectangleBorder : public CelluloZoneRectangle {
    Q_OBJECT

public:
    CelluloZoneRectangleBorder();

    /**
     * @brief Calculate whether the robot lies on the border of this rectangular zone (given the zone's margin)
     * @return 1 if the robot is on the border of this rectangular zone (given the zone's margin) 0 otherwise
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);

};

#endif // CELLULOZONERECTANGLE_H
