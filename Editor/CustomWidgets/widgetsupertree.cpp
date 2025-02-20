/*
    RPG Paper Maker Copyright (C) 2017-2021 Wano

    RPG Paper Maker engine is under proprietary license.
    This source code is also copyrighted.

    Use Commercial edition for commercial use of your games.
    See RPG Paper Maker EULA here:
        http://rpg-paper-maker.com/index.php/eula.
*/

#include <QHeaderView>
#include <QMessageBox>
#include "widgetsupertree.h"
#include "superlistitem.h"
#include "rpm.h"
#include "common.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

WidgetSuperTree::WidgetSuperTree(QWidget *parent) :
    QTreeView(parent),
    p_model(nullptr),
    m_newItemInstance(nullptr),
    m_copiedItem(nullptr),
    m_canBeEmpty(true),
    m_updateId(false),
    m_hasContextMenu(true),
    m_canBeControled(true),
    m_canMove(true),
    m_canCreateDelete(true),
    m_canSameName(true),
    m_pasteBeforeSelected(false)
{
    this->setIndentation(0);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDefaultDropAction(Qt::CopyAction);
    setCanMove(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_contextMenuCommonCommands = ContextMenuList::createContextCommand(this);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
}

WidgetSuperTree::~WidgetSuperTree()
{
    delete m_contextMenuCommonCommands;
    if (m_newItemInstance != nullptr)
        delete m_newItemInstance;

    if (m_copiedItem != nullptr)
        delete m_copiedItem;
}

QStandardItemModel *WidgetSuperTree::getModel() const { return p_model; }

void WidgetSuperTree::setCanBeEmpty(bool b){ m_canBeEmpty = b; }

void WidgetSuperTree::setUpdateId(bool b) { m_updateId = b; }

void WidgetSuperTree::setHasContextMenu(bool b) { m_hasContextMenu = b; }

void WidgetSuperTree::setCanBeControled(bool b) { m_canBeControled = b; }

void WidgetSuperTree::setCanMove(bool b) {
    m_canMove = b;
    this->setAcceptDrops(b);
    this->setDragEnabled(b);
    this->setDropIndicatorShown(b);
}

void WidgetSuperTree::setCanCreateDelete(bool b) {
    m_canCreateDelete = b;
    m_contextMenuCommonCommands->canNew(b);
    m_contextMenuCommonCommands->canEdit(b);
    m_contextMenuCommonCommands->canCopy(b);
    m_contextMenuCommonCommands->canPaste(b);
    m_contextMenuCommonCommands->canDelete(b);
}

void WidgetSuperTree::setCanSameName(bool b)
{
    m_canSameName = b;
}

void WidgetSuperTree::setPasteBeforeSelected(bool b)
{
    m_pasteBeforeSelected = b;
}

void WidgetSuperTree::initializeModel(QStandardItemModel* m){
    p_model = m;
    this->setModel(m);

    // Connect for context menu
    connect(this->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,
            SLOT(onSelectionChanged(QModelIndex,QModelIndex)));
}

void WidgetSuperTree::initializeNewItemInstance(SuperListItem* item){
    m_newItemInstance = item;
}

// -------------------------------------------------------

void WidgetSuperTree::updateAllModelRow() {
    QStandardItemModel *model = getModel();
    int i, l;

    if (model != nullptr) {
        for (i = 0, l = model->invisibleRootItem()->rowCount(); i < l - 1; i++) {
            reinterpret_cast<SuperListItem *>(model->item(i)->data().value<
                quintptr>())->updateModelRow(model, i);
        }
    }
}

// -------------------------------------------------------

void WidgetSuperTree::forceKeyPress(QKeyEvent *event)
{
    this->keyPressEvent(event);
}

// -------------------------------------------------------

void WidgetSuperTree::updateAbsoluteAllNodesString() {
    this->updateAllNodesString(this->getModel()->invisibleRootItem());
}

// -------------------------------------------------------
//
//  CONTEXT MENU
//
// -------------------------------------------------------

void WidgetSuperTree::setItem(QStandardItem *selected, SuperListItem* super) {
    SuperListItem* previous = reinterpret_cast<SuperListItem *>(selected->data()
        .value<quintptr>());

    QStandardItem* root = getRootOfItem(selected);
    if (m_updateId) {
        super->setId(previous == nullptr ? getNewId(p_model) : previous->id());
    }
    QList<QStandardItem*> row = super->getModelRow();
    int index = selected->row();
    if (previous != nullptr)
        root->removeRow(index);
    root->insertRow(index, row);
    setCurrentIndex((previous == nullptr ? selected : row.at(0))->index());
    emit needsUpdateJson(super);
    emit modelUpdated();
}

// -------------------------------------------------------

void WidgetSuperTree::newItem(QStandardItem* selected)
{
    if (m_newItemInstance != nullptr)
    {
        SuperListItem* super = m_newItemInstance->createCopy();
        super->setIsNew(true);
        emit beforeOpeningWindow();
        if (super->openDialog()){
            QStandardItem* root = getRootOfItem(selected);
            int index = selected->row();
            addNewItem(super, root, index);
        }
        else
            delete super;
        super->setIsNew(false);
        emit windowClosed();
    }
}

// -------------------------------------------------------

void WidgetSuperTree::editItem(QStandardItem *selected){
    SuperListItem *super = reinterpret_cast<SuperListItem *>(selected->data()
        .value<quintptr>());
    int previousID, newID;
    previousID = super->id();
    emit beforeOpeningWindow();
    if (super->openDialog())
    {
        newID = super->id();
        if (previousID != newID)
        {
            emit idChanged(previousID, newID);
        }
        setItem(selected, super);
    }
    emit windowClosed();
}

// -------------------------------------------------------

void WidgetSuperTree::copyItem(QStandardItem* selected)
{
    SuperListItem *super = reinterpret_cast<SuperListItem *>(selected->data()
        .value<quintptr>());
    if (super != nullptr)
    {
        if (m_copiedItem == nullptr)
        {
            m_copiedItem = super->createCopy();
        } else
        {
            m_copiedItem->setCopy(*super);
        }
        m_copiedSelected = selected;
    } else
    {
        m_copiedSelected = nullptr;
    }
}

// -------------------------------------------------------

void WidgetSuperTree::pasteItem(QStandardItem* selected)
{
    if (m_copiedItem != nullptr) {
        SuperListItem* super = m_copiedItem->createCopy();
        if (!m_canSameName) {
            bool test = true;
            QString name;
            while (test) {
                test = false;
                name = super->name();
                for (int i = 0, l = p_model->invisibleRootItem()->rowCount(); i
                     < l; i++)
                {
                    SuperListItem *superOther = SuperListItem::getItemModelAt(
                        p_model, i);
                    if (superOther != nullptr && name == superOther->name())
                    {
                        test = true;
                        super->setName(name + "_copy");
                        break;
                    }
                }
            }
        }
        int id = super->id();
        if (m_updateId)
        {
            id = this->getNewId(p_model);
            super->setId(id);
        }
        emit pastingItem(m_copiedItem, super, selected->row());
        if (super->id() == -2) { // (see states where we don't want to duplicate)
            QMessageBox::critical(nullptr, RPM::translate(Translations::ERROR_MESSAGE),
                RPM::translate(Translations::IMPOSSIBLE_TO_PASTE_ITEM) + RPM
                ::SPACE + Common::getFormatNumber(id) + RPM::DOT);
        } else {
            if (m_pasteBeforeSelected)
            {
                this->addNewItem(super, p_model->invisibleRootItem(), selected->index().row());
            } else
            {
                this->setItem(selected, super);
            }
        }
    }
}

// -------------------------------------------------------

void WidgetSuperTree::deleteItem(QStandardItem* selected){
    SuperListItem *super = reinterpret_cast<SuperListItem *>(selected->data()
        .value<quintptr>());
    int row;

    // Can't delete empty node
    if (super != nullptr){
        // If can be empty
        if (m_canBeEmpty || p_model->invisibleRootItem()->rowCount() > 2){
            QStandardItem* root = getRootOfItem(selected);
            row = selected->row();
            root->removeRow(row);
            emit deletingItem(super, row);
            delete super;
            emit needsUpdateJson(nullptr);
            emit modelUpdated();
        }
    }
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

// -------------------------------------------------------
//  getSelected: returns the current selected item of the tree commands

QStandardItem* WidgetSuperTree::getSelected() const{
    return p_model->item(this->selectionModel()->currentIndex().row());
}

// -------------------------------------------------------
//  getRootOfCommand: returns the root of the current selected command

QStandardItem* WidgetSuperTree::getRootOfItem(QStandardItem* selected){
    return (selected->parent() == nullptr) ? p_model->invisibleRootItem()
                                           : selected->parent();
}

// -------------------------------------------------------

void WidgetSuperTree::updateAllNodesString(QStandardItem *item) {
    QList<QStandardItem *> row;
    QStandardItem *itemRow;
    int i, j, l, ll;
    for (i = 0, l = item->rowCount(); i < l; i++) {
        updateAllNodesString(item->child(i));
        SuperListItem *super = reinterpret_cast<SuperListItem *>(item->child(i)
            ->data().value<quintptr>());
        if (super != nullptr) {
            row = super->getModelRow();
            for (j = 0, ll = row.size(); j < ll; j++) {
                itemRow = row.at(j);
                item->child(i, j)->setText(itemRow->text());
                delete itemRow;
            }
        }
    }
}

// -------------------------------------------------------

int WidgetSuperTree::getNewId(QStandardItemModel *model, int offset){
    int id;
    for (id = 1; id < model->invisibleRootItem()->rowCount(); id++){
        bool test = false;
        int length = model->invisibleRootItem()->rowCount() - offset;
        for (int j = 0; j < length; j++){
            SuperListItem *super = reinterpret_cast<SuperListItem *>(model->item
                (j)->data().value<quintptr>());
            if (id == super->id()){
                test = true;
                break;
            }
        }
        if (!test) break;
    }

    return id;
}

// -------------------------------------------------------

QStandardItem * WidgetSuperTree::addNewItem(SuperListItem* super, QStandardItem
    *root, int index)
{
    if (m_updateId) {
        super->setId(getNewId(p_model));
    }
    QList<QStandardItem*> row = super->getModelRow();
    root->insertRow(index, row);
    QModelIndex modelIndex = p_model->index(index, 0);
    setCurrentIndex(modelIndex);
    emit needsUpdateJson(super);
    emit modelUpdated();
    return row.at(0);
}

// -------------------------------------------------------

void WidgetSuperTree::updateKeyboardUpDown(int offset) {
    QStandardItem *item;
    int row, column;

    item = getSelected();
    if (item != nullptr) {
        row = item->row();
        column = item->column();
        item = this->getModel()->item(row + offset, column);
        if (item != nullptr) {
            this->selectionModel()->clear();
            this->selectionModel()->setCurrentIndex(item->index(),
                QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }
}

// -------------------------------------------------------
//
//  EVENTS
//
// -------------------------------------------------------

void WidgetSuperTree::keyPressEvent(QKeyEvent *event) {
    if (Common::isPressingEnter(event))
    {
        emit tryingEdit();
    }
    if (m_hasContextMenu || !m_canBeControled)
    {
        // Forcing shortcuts
        QKeySequence seq = Common::getKeySequence(event);
        QKeySequence seqBackspace(Qt::Key_Backspace);
        QList<QAction*> actions = m_contextMenuCommonCommands->actions();
        QAction *action;
        action = actions.at(6);
        if ((action->shortcut().matches(seq) || seq.matches(seqBackspace)) &&
            action->isEnabled())
        {
            contextDelete();
            return;
        }
        if (m_canBeControled)
        {
            action = actions.at(1);
            if (Common::isPressingEnter(event) && action->isEnabled())
            {
                contextEdit();
                return;
            }
            action = actions.at(0);
            if (Common::isPressingEnter(event) && action->isEnabled())
            {
                contextNew();
                return;
            }
            action = actions.at(3);
            if (action->shortcut().matches(seq) && action->isEnabled())
            {
                contextCopy();
                return;
            }
            action = actions.at(4);
            if (action->shortcut().matches(seq) && action->isEnabled())
            {
                contextPaste();
                return;
            }
        }
    }
    int key = event->key();
    if (key == Qt::Key_Up)
    {
        updateKeyboardUpDown(-1);
    }
    if (key == Qt::Key_Down)
    {
        updateKeyboardUpDown(1);
    }
    emit keyPressed(event);
}

// -------------------------------------------------------



// -------------------------------------------------------
//  mouseDoubleClickEvent: when double clicking, edit if existing command,
//  if not create a new one

void WidgetSuperTree::mouseDoubleClickEvent(QMouseEvent* event){
    if (m_canBeControled){
        if (event->button() == Qt::MouseButton::LeftButton){
            QStandardItem* selected = getSelected();
            if (selected != nullptr){
                SuperListItem *super = reinterpret_cast<SuperListItem *>(
                    selected->data().value<quintptr>());
                // if empty, create a new command
                if (super == nullptr)
                {
                    newItem(selected);
                }
                // Else only edit the command
                else
                {
                    editItem(selected);
                }
            }
        }
    }
    QTreeView::mouseDoubleClickEvent(event);
}

// -------------------------------------------------------

void WidgetSuperTree::dropEvent(QDropEvent *event) {
    QPoint pos = event->pos();
    QModelIndex index = this->indexAt(pos);
    QStandardItem *item = this->getModel()->itemFromIndex(index);
    QStandardItem *selected = this->getSelected();
    QList<QStandardItem *> newItems;
    SuperListItem *super;

    if (item == nullptr) {
        item = this->getModel()->item(this->getModel()->rowCount() - 1);
    }
    super = reinterpret_cast<SuperListItem *>(selected->data().value<quintptr>());
    this->getModel()->removeRow(selected->row());

    int row = item->column() == 0 ? item->row() : item->row() + 1;
    if (super == nullptr) {
        selected = SuperListItem::getEmptyItem();
        this->getModel()->insertRow(row, selected);
    } else {
        this->getModel()->insertRow(row, super->getModelRow());
    }
    emit modelUpdated();
}

// -------------------------------------------------------
//
//  SLOTS
//
// -------------------------------------------------------

void WidgetSuperTree::onSelectionChanged(QModelIndex index, QModelIndex) {
    QStandardItem* selected = p_model->itemFromIndex(index);
    SuperListItem* super = nullptr;
    if (selected != nullptr)
    {
        super = reinterpret_cast<SuperListItem *>(selected->data().value<
            quintptr>());
    }
    m_contextMenuCommonCommands->canEdit(m_canCreateDelete && super != nullptr);
}

// -------------------------------------------------------
//
//  CONTEXT MENU SLOTS
//
// -------------------------------------------------------

void WidgetSuperTree::showContextMenu(const QPoint & p){
    if (m_hasContextMenu) {
        QStandardItem *selected = getSelected();
        if (selected != nullptr)
        {
            this->updateContextMenu();
            m_contextMenuCommonCommands->showContextMenu(p);
        }
    }
}

// -------------------------------------------------------

void WidgetSuperTree::updateContextMenu()
{

}

// -------------------------------------------------------

void WidgetSuperTree::contextNew()
{
    QStandardItem *item = this->getSelected();
    if (item != nullptr)
    {
        newItem(getSelected());
    }
}

// -------------------------------------------------------

void WidgetSuperTree::contextEdit()
{
    QStandardItem *item = this->getSelected();
    if (item != nullptr)
    {
        editItem(getSelected());
    }
}

// -------------------------------------------------------

void WidgetSuperTree::contextCopy()
{
    QStandardItem *item = this->getSelected();
    if (item != nullptr)
    {
        copyItem(getSelected());
    }
}

// -------------------------------------------------------

void WidgetSuperTree::contextPaste()
{
    QStandardItem *item = this->getSelected();
    if (item != nullptr)
    {
        pasteItem(getSelected());
    }
}

// -------------------------------------------------------

void WidgetSuperTree::contextDelete()
{
    QStandardItem *item = this->getSelected();
    if (item != nullptr)
    {
        deleteItem(getSelected());
    }
}
