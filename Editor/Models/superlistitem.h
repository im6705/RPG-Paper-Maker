/*
    RPG Paper Maker Copyright (C) 2017-2021 Wano

    RPG Paper Maker engine is under proprietary license.
    This source code is also copyrighted.

    Use Commercial edition for commercial use of your games.
    See RPG Paper Maker EULA here:
        http://rpg-paper-maker.com/index.php/eula.
*/

#ifndef SUPERLISTITEM_H
#define SUPERLISTITEM_H

#include <QString>
#include <QMetaType>
#include <QStandardItem>
#include <QComboBox>
#include "serializable.h"
#include "picturekind.h"

// -------------------------------------------------------
//
//  CLASS SuperListItem
//
//  All the classes inheriting this can be an item of a
//  SuperList.
//
// -------------------------------------------------------

class SuperListItem : public Serializable
{
public:
    static const QString JSON_ID;
    static const QString JSON_NAME;
    static const int DEFAULT_ID;
    static const QString DEFAULT_NAME;

    SuperListItem();
    virtual ~SuperListItem();
    SuperListItem(int i, QString n = QString(), bool datas = false, bool
        displayID = true, bool isNew = false);
    int id() const;
    bool isDefault();
    virtual void setId(int i);
    virtual QString name() const;
    virtual void setName(QString n);
    void setDisplayID(bool b);
    bool isNew() const;
    void setIsNew(bool isNew);

    virtual QString toString() const;
    virtual QString toStringName() const;
    static QString pathIconRed;
    static QString pathIconBlue;
    static QString beginningText;
    virtual QString idToString() const;
    virtual void setDefault();
    virtual bool openDialog();
    virtual SuperListItem* createCopy() const;
    virtual void setCopy(const SuperListItem& item);
    virtual QList<QStandardItem*> getModelRow() const;
    virtual void updateModelRow(QStandardItemModel* model, int row);
    static bool containsName(QStandardItemModel *model, QString name);
    static SuperListItem * getItemModelAt(QStandardItemModel *model, int i);
    static void deleteModel(QStandardItemModel *model, bool deleteModel = true);
    static void deleteModelItem(QStandardItem *item);
    static void deleteModelTree(QStandardItem* item);
    static int getIndexById(QStandardItem* item, int id, bool first = false);
    static int getIdByIndex(QStandardItemModel* model, int index);
    static SuperListItem* getById(QStandardItem* item, int id,
                                  bool first = true);
    static QStandardItem * getItemByID(QStandardItem *item, int id, bool first =
        true);
    static SuperListItem* getByIndex(QStandardItemModel* model, int index);
    static void fillComboBox(QComboBox* comboBox, QStandardItemModel* model,
        bool showID = true, bool nameOnly = false, bool showIcon = false);
    static void copyModel(QStandardItemModel* model,
                          QStandardItemModel* baseModel);
    static void replaceModel(QStandardItemModel* model, QStandardItemModel
        *baseModel);
    static SuperListItem* getnewInstance(PictureKind kind);
    static QStandardItem * getEmptyItem();
    static void copy(QStandardItemModel *model, QStandardItemModel *modelToCopy);
    static void readTree(QStandardItemModel *model, SuperListItem *newInstance,
        const QJsonObject &json, const QString &name);
    static void writeTree(QStandardItemModel *model, QJsonObject &json, const
        QString &name);
    static void readList(QStandardItemModel *model, SuperListItem *newInstance,
        const QJsonObject &json, const QString &name);
    static void writeList(QStandardItemModel *model, QJsonObject &json, const
        QString &name);
    static void getWeaponsList(QStandardItemModel *model, int id);
    static void getArmorsList(QStandardItemModel *model, int id);
    static void updateWeaponsArmorsList(QStandardItemModel *model,
        QStandardItemModel *modelWeaponArmor, QStandardItemModel
        *modelWeaponArmorKind, int id);
    static void removeEmptyInTree(QStandardItemModel *model);
    static void updateModelToString(QStandardItemModel *model);
    void reset();
    virtual void getIcon(QIcon &icon);

    virtual void read(const QJsonObject &json);
    void readId(const QJsonObject &json);
    virtual void write(QJsonObject &json) const;
    void writeId(QJsonObject &json) const;

protected:
    int p_id;
    QString p_name;
    bool m_containsDatas;
    bool m_displayID;
    bool m_isNew;
};

Q_DECLARE_METATYPE(SuperListItem)

#endif // SUPERLISTITEM_H
