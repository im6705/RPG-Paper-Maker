/*
    RPG Paper Maker Copyright (C) 2017-2021 Wano

    RPG Paper Maker engine is under proprietary license.
    This source code is also copyrighted.

    Use Commercial edition for commercial use of your games.
    See RPG Paper Maker EULA here:
        http://rpg-paper-maker.com/index.php/eula.
*/

#include "widgetpicturepreview.h"
#include "rpm.h"
#include <QPainter>

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

WidgetPicturePreview::WidgetPicturePreview(QWidget *parent) :
    QWidget(parent),
    m_picture(nullptr),
    m_selectionRectangle(new WidgetSelectionRectangle),
    m_chooseRect(false),
    m_indexX(0),
    m_indexY(0),
    m_isLimitIndex(true)
{

}

WidgetPicturePreview::~WidgetPicturePreview()
{
    delete m_selectionRectangle;
}

void WidgetPicturePreview::setKind(PictureKind kind) { m_kind = kind; }

SystemPicture * WidgetPicturePreview::picture() const
{
    return m_picture;
}

void WidgetPicturePreview::setPicture(SystemPicture *picture)
{
    m_picture = picture;
}

void WidgetPicturePreview::setChooseRect(bool b) { m_chooseRect = b; }

int WidgetPicturePreview::indexX() const { return m_indexX; }

void WidgetPicturePreview::setIndexX(int i) {
    if (m_isLimitIndex)
    {
        int frames = RPM::get()->project()->gameDatas()->systemDatas()
            ->framesAnimation();
        if (i >= frames)
        {
            i = frames - 1;
        }
    }
    m_indexX = i;
    this->updateRectangle();
}

int WidgetPicturePreview::indexY() const { return m_indexY; }

void WidgetPicturePreview::setIndexY(int i) {
    if (m_isLimitIndex)
    {
        int max = m_picture == nullptr ? 4 : m_picture->getRows();
        if (i >= max - 1)
        {
            i = max - 1;
        }
    }
    m_indexY = i;
    this->updateRectangle();
}

void WidgetPicturePreview::setIsLimitIndex(bool isLimitIndex)
{
    m_isLimitIndex = isLimitIndex;
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void WidgetPicturePreview::setImage(QString path){
    m_image.load(path);
    updateImageSize();
}

// -------------------------------------------------------

void WidgetPicturePreview::setNoneImage(){
    this->setPicture(nullptr);
    m_image = QImage();
    updateImageSize();
}

// -------------------------------------------------------

void WidgetPicturePreview::updateImageSize(){

    float coef = RPM::coefReverseSquareSize();

    // Set selector
    if (m_chooseRect && !m_image.isNull()){
        int frames = RPM::get()->project()->gameDatas()->systemDatas()
                ->framesAnimation();

        switch (m_kind){
        case PictureKind::Characters:
            m_selectionRectangle->setSquareWidth(m_image.width() * coef / frames);
            m_selectionRectangle->setSquareHeight(m_image.height() * coef/ (
                m_picture == nullptr ? 4 : m_picture->getRows()));
            this->updateRectangleCharacter();
            break;
        case PictureKind::Icons:
            m_selectionRectangle->setSquareWidth(RPM::get()->project()->gameDatas()
                ->systemDatas()->iconsSize());
            m_selectionRectangle->setSquareHeight(RPM::get()->project()->gameDatas()
                ->systemDatas()->iconsSize());
            this->updateRectangleCharacter();
            break;
        case PictureKind::Facesets:
            m_selectionRectangle->setSquareWidth(RPM::get()->project()->gameDatas()
                ->systemDatas()->facesetsSize());
            m_selectionRectangle->setSquareHeight(RPM::get()->project()->gameDatas()
                ->systemDatas()->facesetsSize());
            this->updateRectangleCharacter();
            break;
        default:
            break;
        }
    }

    // Set size
    if (m_chooseRect && !m_image.isNull() &&
        (m_kind == PictureKind::Characters || m_kind == PictureKind::Tilesets))
    {
        m_image = m_image.scaled(m_image.width() * coef, m_image.height() * coef);
    }
    this->setGeometry(0, 0, m_image.width(), m_image.height());
    setFixedSize(m_image.width(), m_image.height());
}

// -------------------------------------------------------

void WidgetPicturePreview::updateRectangle()
{
    switch (m_kind){
    case PictureKind::Characters:
    case PictureKind::Icons:
    case PictureKind::Facesets:
        updateRectangleCharacter(); break;
    default:
        break;
    }
}

// -------------------------------------------------------

void WidgetPicturePreview::updateRectangleCharacter(){
    m_selectionRectangle->setRectangle(m_indexX, m_indexY, 1, 1);
}

// -------------------------------------------------------
//
//  EVENTS
//
// -------------------------------------------------------

void WidgetPicturePreview::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::MouseButton::LeftButton){
        m_selectionRectangle->makeFirstSelection(event->pos().x(),
                                                 event->pos().y(),
                                                 1.0f);
        QRect rect;
        m_selectionRectangle->getCoefRect(rect);
        m_indexX = rect.x();
        m_indexY = rect.y();
        this->repaint();
    }
}

// -------------------------------------------------------

void WidgetPicturePreview::paintEvent(QPaintEvent *){
    QPainter painter(this);

    painter.fillRect(QRect(0, 0, m_image.width(), m_image.height()), RPM
        ::COLOR_ALMOST_TRANSPARENT);
    painter.drawImage(0, 0, m_image);

    if (m_chooseRect && (m_kind == PictureKind::Characters || m_kind == PictureKind
        ::Icons || m_kind == PictureKind::Facesets))
    {
        m_selectionRectangle->draw(painter);
    }
}
