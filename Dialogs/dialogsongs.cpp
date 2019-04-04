/*
    RPG Paper Maker Copyright (C) 2017-2019 Marie Laporte

    RPG Paper Maker engine is under LGPL-3 license.

    Commercial license for commercial use of your games:
        https://creativecommons.org/licenses/by-nc/4.0/.

    See more information here: http://rpg-paper-maker.com/index.php/downloads.
*/

#include "dialogsongs.h"
#include "ui_dialogsongs.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

DialogSongs::DialogSongs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSongs)
{
    ui->setupUi(this);
    

    ui->widgetSongs->initializePrimitives(nullptr, nullptr);

    // Tree model
    ui->treeView->setModel(createFoldersModel());
    ui->treeView->expandAll();
    connect(ui->treeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(on_folderSelected(QModelIndex,QModelIndex)));

    // Keep space when hiding widget
    QSizePolicy sp_retain;
    sp_retain = ui->widgetSongs->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    ui->widgetSongs->setSizePolicy(sp_retain);
}

DialogSongs::~DialogSongs()
{
    delete ui->treeView->model();
    delete ui;
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

QStandardItemModel* DialogSongs::createFoldersModel() const{
    QStandardItemModel* model = new QStandardItemModel;
    QStandardItem* itemSongs;
    QIcon icon(":/icons/Ressources/dir.png");
    QList<QString> listNames;
    QList<SongKind> listEnums;

    // Pictures
    itemSongs = new QStandardItem;
    itemSongs->setData(QVariant::fromValue(
                      reinterpret_cast<SongKind>(SongKind::None)));
    itemSongs->setIcon(icon);
    itemSongs->setText("Songs");
    model->appendRow(itemSongs);

    // HUD
    listNames << "Musics" << "BackgroundSounds" << "Sounds" << "MusicEffects";
    listEnums << SongKind::Music << SongKind::BackgroundSound << SongKind::Sound
              << SongKind::MusicEffect;
    addfolders(icon, itemSongs, listNames, listEnums);

    return model;
}

// -------------------------------------------------------

void DialogSongs::addfolders(QIcon &icon,
                                QStandardItem* root,
                                QList<QString> &names,
                                QList<SongKind> &enums) const
{
    QStandardItem* item;

    for (int i = 0; i < names.size(); i++){
        item = new QStandardItem;
        item->setData(QVariant::fromValue(
                          reinterpret_cast<SongKind>(enums.at(i))));
        item->setIcon(icon);
        item->setText(names.at(i));
        root->appendRow(item);
    }
}

// -------------------------------------------------------
//
//  SLOTS
//
// -------------------------------------------------------

void DialogSongs::on_folderSelected(const QModelIndex& current,
                                    const QModelIndex&)
{
    QStandardItemModel* model = (QStandardItemModel*) ui->treeView->model();
    QStandardItem* item = model->itemFromIndex(current);
    SongKind k = item->data().value<SongKind>();
    ui->widgetSongs->setSongKind(k);
}
