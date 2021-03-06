#include "SheetBrowserDJVUItem.h"
#include "document/model.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

SheetBrowserDJVUItem::SheetBrowserDJVUItem(deepin_reader::Page *page) : QGraphicsItem()
{
    m_page = page;
}

SheetBrowserDJVUItem::~SheetBrowserDJVUItem()
{
    if (nullptr != m_page)
        delete m_page;
}

void SheetBrowserDJVUItem::setScale(double scale)
{

}

void SheetBrowserDJVUItem::setRotation(Dr::Rotation rotation)
{

}

QRectF SheetBrowserDJVUItem::boundingRect() const
{
    if (nullptr == m_page)
        return QRectF(0, 0, 0, 0);

    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        return QRectF(0, 0, (double)m_page->size().height() * m_scale / 100.00, (double)m_page->size().width() * m_scale / 100.00);
    default: break;
    }

    return QRectF(0, 0, (double)m_page->size().width() * m_scale / 100.00, (double)m_page->size().height() * m_scale / 100.00);
}

void SheetBrowserDJVUItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (m_image.isNull()) {
        painter->setBrush(Qt::white);
        painter->drawRect(option->rect);
        render(m_scale, m_rotation, false);
    } else
        painter->drawImage(option->rect, m_image);
}

void SheetBrowserDJVUItem::render(double scale, Dr::Rotation rotation, bool readerLater)
{
    if (nullptr == m_page)
        return;

    if (!m_image.isNull() && qFuzzyCompare(scale, m_scale) && rotation == m_rotation)
        return;

    m_scale     = scale;
    m_rotation  = rotation;

    if (!readerLater)
        m_image = m_page->render(72, 72, m_rotation, scale);
    else
        m_image = QImage();

    update();
}
