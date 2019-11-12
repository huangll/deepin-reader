#include "ImageLabel.h"
#include <QPainter>

ImageLabel::ImageLabel(DWidget *parent)
    : DLabel (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void ImageLabel::setSelect(const bool &select)
{
    m_bSelect = select;
    update();
}

void ImageLabel::paintEvent(QPaintEvent *e)
{
    int local = 0;
    int width = this->width();
    int heigh = this->height();

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (m_bSelect) {
        local = 1;
        width -= 2;
        heigh -= 2;

        painter.setPen(QPen(QColor(QString("#0081FF")), 3, Qt::SolidLine));
    } else {

        painter.setPen(QPen(QColor::fromRgbF(0, 0, 0, 0.05), 1, Qt::SolidLine));
    }
    QRectF rectangle(local, local, width, heigh);
    painter.drawRoundedRect(rectangle, 8, 8);

    DLabel::paintEvent(e);
}