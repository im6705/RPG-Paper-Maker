/*
    RPG Paper Maker Copyright (C) 2017-2021 Wano

    RPG Paper Maker engine is under proprietary license.
    This source code is also copyrighted.

    Use Commercial edition for commercial use of your games.
    See RPG Paper Maker EULA here:
        http://rpg-paper-maker.com/index.php/eula.
*/

#ifndef SYSTEMDETECTION_H
#define SYSTEMDETECTION_H

#include <QMetaType>
#include "position3d.h"
#include "objects3d.h"

class Map;

// -------------------------------------------------------
//
//  CLASS SystemDetection
//
//  A particulary detection (datas).
//
// -------------------------------------------------------

class SystemDetection : public SuperListItem
{
public:
    static const QString JSON_FIELD_LEFT;
    static const QString JSON_FIELD_RIGHT;
    static const QString JSON_FIELD_TOP;
    static const QString JSON_FIELD_BOT;
    static const QString JSON_BOXES;
    static const QString JSON_BOXES_LENGTH_SQUARES;
    static const QString JSON_BOXES_LENGTH_PIXELS;
    static const QString JSON_BOXES_WIDTH_SQUARES;
    static const QString JSON_BOXES_WIDTH_PIXELS;
    static const QString JSON_BOXES_HEIGHT_SQUARES;
    static const QString JSON_BOXES_HEIGHT_PIXELS;
    static const int DEFAULT_FIELD_LEFT;
    static const int DEFAULT_FIELD_RIGHT;
    static const int DEFAULT_FIELD_TOP;
    static const int DEFAULT_FIELD_BOT;
    static const int DEFAULT_CAMERA_DISTANCE;
    static const int DEFAULT_CAMERA_HORIZONTAL_ANGLE;

    SystemDetection();
    SystemDetection(int i, QString n, int fl = DEFAULT_FIELD_LEFT, int fr =
        DEFAULT_FIELD_RIGHT, int ft = DEFAULT_FIELD_TOP, int fb =
        DEFAULT_FIELD_BOT);
    virtual ~SystemDetection();

    int fieldLeft() const;
    void setFieldLeft(int f);
    int fieldRight() const;
    void setFieldRight(int f);
    int fieldTop() const;
    void setFieldTop(int f);
    int fieldBot() const;
    void setFieldBot(int f);
    void setCurrentLengthSquares(int v);
    void setCurrentLengthPixels(double v);
    void setCurrentWidthSquares(int v);
    void setCurrentWidthPixels(double v);
    void setCurrentHeightSquares(int v);
    void setCurrentHeightPixels(double v);

    Map * createDetectionMap();
    void getTargetPosition(QVector3D *position) const;
    SystemObject3D * instanciateObject() const;
    void setFront();
    void setSelf();
    void initializeObjects(Objects3D *objects3D, Portion &globalPortion);
    void addObject(Position &position, SystemObject3D *object);
    void deleteObject(Position &position);
    void correctPosition(Position &newPosition, Position &position);
    void removeLimitDetections();
    void generateCircle(int radius, Position &origin);
    void generateRectangle(int length, int width, Position &origin);
    void clearPreview(Objects3D *objects3D);
    void updatePreview(Objects3D *objects3D, Position &position);

    virtual void setDefault();
    virtual bool openDialog();
    virtual SuperListItem * createCopy() const;
    virtual void setCopy(const SuperListItem &super);
    virtual void read(const QJsonObject &json);
    virtual void write(QJsonObject &json) const;

protected:
    int m_fieldLeft;
    int m_fieldRight;
    int m_fieldTop;
    int m_fieldBot;
    int m_currentLengthSquares;
    double m_currentLengthPixels;
    int m_currentWidthSquares;
    double m_currentWidthPixels;
    int m_currentHeightSquares;
    double m_currentHeightPixels;
    QHash<Position, SystemObject3D *> m_boxes;
    Position m_currentPreviewPosition;
    SystemObject3D *m_currentPreviewObject;
};

Q_DECLARE_METATYPE(SystemDetection);

#endif // SYSTEMDETECTION_H
