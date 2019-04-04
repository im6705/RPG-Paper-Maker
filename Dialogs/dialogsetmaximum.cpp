/*
    RPG Paper Maker Copyright (C) 2017-2019 Marie Laporte

    RPG Paper Maker engine is under LGPL-3 license.

    Commercial license for commercial use of your games:
        https://creativecommons.org/licenses/by-nc/4.0/.

    See more information here: http://rpg-paper-maker.com/index.php/downloads.
*/

#include "dialogsetmaximum.h"
#include "ui_dialogsetmaximum.h"
#include "superlistitem.h"
#include <QMessageBox>

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

DialogSetMaximum::DialogSetMaximum(QStandardItemModel* model, int max,
                                   QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetMaximum),
    m_model(model)
{
    ui->setupUi(this);
    

    ui->spinBoxMaximum->setMaximum(max);
    ui->spinBoxMaximum->setValue(model->invisibleRootItem()->rowCount());
}

DialogSetMaximum::~DialogSetMaximum()
{
    delete ui;
}

int DialogSetMaximum::maximum() const { return ui->spinBoxMaximum->value(); }

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

bool DialogSetMaximum::isOrdered(int limit) const{

    // If deleting stuff...
    if (limit < m_model->invisibleRootItem()->rowCount()){
        for (int i = 0; i < limit; i++){
            int id = ((SuperListItem*) m_model->item(i)->data()
                      .value<quintptr>())->id();
            if (id > limit) return false;
        }

    }

    return true;
}

// -------------------------------------------------------

void DialogSetMaximum::accept(){
    if (isOrdered(maximum()))
        QDialog::accept();
    else
        QMessageBox::information(this, "Warning",
                                 "This maximum is incorrect because it will "
                                 "delete IDs. Please reorder your stuff "
                                 "properly.");
}
