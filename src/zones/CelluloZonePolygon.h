#ifndef CELLULOZONEPOLYGON_H
#define CELLULOZONEPOLYGON_H

#include "CelluloZone.h"

/**
 * @brief CelluloZone Base Class for polygonal zones
 */
class CelluloZonePolygon : public CelluloZone {
    Q_OBJECT

public:
    /**
     * @brief Creates a new Cellulo polygonal zone
     */
    CelluloZonePolygon();

    virtual QVariantMap getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight) = 0;

    /**
     * @brief Calculate the computation of this polygonal zone according to the robot's pose
     * @return Computation's result for this zone and this robot's pose
     */
    Q_INVOKABLE virtual float calculate(float xRobot, float yRobot, float thetaRobot) = 0;

    /**
     * @brief Write the zone infos to the given json Object
     * @param QJsonObject json object to be written
     */
    virtual void write(QJsonObject &json) = 0;


    /**
     * @brief Read the zone infos from the given json Object
     * @param json json object to be read
     */
    virtual void read(const QJsonObject &json) = 0;


protected:
    QList<QPointF> pointsQt;                ///< points of the polygon
    qreal minX;                             ///< minimal x position from all polygon's point
    qreal maxX;                             ///< maximal x position from all polygon's point
    qreal minY;                             ///< minimal x position from all polygon's point
    qreal maxY;                             ///< maximal y position from all polygon's point

    /**
     * @brief Gets the polygon's points
     * @return List of polygon's points
     */
    QList<QPointF> getPointsQt() const{ return pointsQt; }
    void setPointsQt(const QList<QPointF> &newPointsQt);

    /**
     * @brief setMaxMinOuterRectangle set the max and min points to determine the outer rectangle of this polygonal zone
     * @param pointsQt polygon's points
     * @param minX minimal x position from all polygon's point
     * @param maxX maximal x position from all polygon's point
     * @param minY minimal y position from all polygon's point
     * @param maxY maximal y position from all polygon's point
     */
    void setMaxMinOuterRectangle(const QList<QPointF> &pointsQt, qreal *minX, qreal *maxX, qreal *minY, qreal *maxY);

    /**
     * @brief pointInPoly Detect if a points is inside this polygonal zone
     * @param xRobot x position of the point to be checked
     * @param yRobot y position of the point to be checked
     * @return 1 if the point is inside this polygonal zone, 0 otherwise
     */
    float pointInPoly(float xRobot, float yRobot);


};


/**
 * @brief CelluloZone Base Class for irregular polygonal zones
 */
class CelluloZoneIrregularPolygon : public CelluloZonePolygon {
   Q_OBJECT
   Q_PROPERTY (QList<QVariant> vertices READ getVertices WRITE setVertices NOTIFY verticesChanged)

public:
    /**
     * @brief Gets the vertices (set via QML) of the irregular polygon
     * @return List of vertices (set via QML) of the irregular polygon
     */
    QList<QVariant> getVertices() { return vertices; }
    Q_INVOKABLE void setVertices(QList<QVariant> newPoints) {
        if(newPoints!=vertices){
            vertices = newPoints;
            emit(verticesChanged());
            setPointsQt(convertQVariantToQPointF());
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
     * @brief Calculate the computation of this irregular polygonal zone according to the robot's pose
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
   QList<QVariant> vertices;                ///< vertices of the polygon set via QML

   /**
    * @brief convertQVariantToQPointF convert vertices set via QML into points visible by the backend
    * @return List of points visible by the backend
    */
   QList<QPointF> convertQVariantToQPointF();

signals:
   /**
    * @brief Emitted when vertices of the irregular polygonal zone changes
    */
    void verticesChanged();


};


/**
 * @brief CelluloZone Specific Class for irregular polygonal zones inner determination
 */
class CelluloZoneIrregularPolygonInner : public CelluloZoneIrregularPolygon {
    Q_OBJECT

public:
    CelluloZoneIrregularPolygonInner();

    /**
     * @brief Calculate whether the robot lies inside this irregular polygonal zone
     * @return 1 if the robot is inside this irregular polygonal zone 0 otherwise
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);
};



class CelluloZoneRegularPolygon : public CelluloZonePolygon {
    Q_OBJECT
    Q_PROPERTY(float numEdges WRITE setNumEdges READ getNumEdges NOTIFY numEdgesChanged)
    Q_PROPERTY(float x WRITE setX READ getX NOTIFY xChanged)
    Q_PROPERTY(float y WRITE setY READ getY NOTIFY yChanged)
    Q_PROPERTY(float r WRITE setR READ getR NOTIFY rChanged)
    Q_PROPERTY(float rotAngle WRITE setRotAngle READ getRotAngle NOTIFY rotAngleChanged)


public:
    /**
     * @brief Creates a new Cellulo regular polygonal zone
     */
    CelluloZoneRegularPolygon();

    /**
     * @brief get the properties of this zone in rationalized form
     * @param realPlaygroundWidth real playground width
     * @param realPlaygroundHeight real playground height
     * @return map of rationalized properties
     */
    QVariantMap getRatioProperties(float realPlaygroundWidth, float realPlaygroundHeight);

    /**
     * @brief Gets the number of edges of the regular polygon
     * @return The number of edges of the regular polygon
     */
    float getNumEdges() { return numEdges; }
    void setNumEdges(float newNumEdge) {
        if(newNumEdge!=numEdges){
            numEdges = newNumEdge;
            emit(numEdgesChanged());
            if(numEdges>2){
            setPointsQt(createPolygonPointsFromOuterCircle());
            }
        }
    }

    /**
     * @brief Gets the x position of regular polygon definer circle's center
     * @return X position of regular polygon definer circle's center
     */
    float getX() { return x; }
    void setX(float newX) {
        if(newX!=x){
            x = newX;
            emit(xChanged());
            if(numEdges>2){
            setPointsQt(createPolygonPointsFromOuterCircle());
            }
        }
    }

    /**
     * @brief Gets the y position of regular polygon definer circle's center
     * @return Y position of regular polygon definer circle's center
     */
    float getY() { return y; }
    void setY(float newY) {
        if(newY!=y){
            y = newY;
            emit(yChanged());
            if(numEdges>2){
            setPointsQt(createPolygonPointsFromOuterCircle());
            }
        }
    }

    /**
     * @brief Gets the radius of the regular polygon definer circle
     * @return R the radius of the regular polygon definer circle
     */
    float getR() { return r; }
    void setR( float newR) {
        if(newR!=r){
            r = newR;
            emit(rChanged());
            if(numEdges>2){
            setPointsQt(createPolygonPointsFromOuterCircle());
            }
        }
    }

    /**
     * @brief Gets the rotational angle of the regular polygon
     * @return The rotational angle of the regular polygon
     */
    float getRotAngle() { return rotAngle; }
    void setRotAngle( float newRotAngle) {
        if(newRotAngle!=rotAngle){
            rotAngle = newRotAngle;
            emit(rotAngleChanged());
            if(numEdges>2){
            setPointsQt(createPolygonPointsFromOuterCircle());
            }
        }
    }

    /**
     * @brief Calculate the computation of this regular polygonal zone according to the robot's pose
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
    float numEdges;                         ///< number of edges of the regular polygon
    float x;                                ///< x position of regular polygon defined circle's center
    float y;                                ///< y position of regular polygon defined circle's center
    float r;                                ///< radius of the regular polygon defined circle
    float rotAngle;                         ///< rotational angle of the regular polygon
private:
    /**
     * @brief Define regular polygon's point following the polygon definer circle, the number of edges and the rotational angle
     * @return points of the regular polygon
     */
    QList<QPointF> createPolygonPointsFromOuterCircle();

signals:
    /**
     * @brief Emitted when the number of edges of the regular polygonal zone changes
     */
    void numEdgesChanged();
    /**
     * @brief Emitted when the x position of regular polygon defined circle's center of the regular polygonal zone changes
     */
    void xChanged();
    /**
     * @brief Emitted when the y position of regular polygon defined circle's center of the regular polygonal zone changes
     */
    void yChanged();
    /**
     * @brief Emitted when the radius of the regular polygon defined circle of the regular polygonal zone changes
     */
    void rChanged();
    /**
     * @brief Emitted when the rotational angle of the regular polygonal zone changes
     */
    void rotAngleChanged();

};

/**
 * @brief CelluloZone Specific Class for regular polygonal zones inner determination
 */
class CelluloZoneRegularPolygonInner : public CelluloZoneRegularPolygon {
    Q_OBJECT

public:
    CelluloZoneRegularPolygonInner();

    /**
     * @brief Calculate whether the robot lies inside this regular polygonal zone
     * @return 1 if the robot is inside this regular polygonal zone 0 otherwise
     */
    Q_INVOKABLE float calculate(float xRobot, float yRobot, float thetaRobot);

};

#endif // CELLULOZONEPOLYGON_H
