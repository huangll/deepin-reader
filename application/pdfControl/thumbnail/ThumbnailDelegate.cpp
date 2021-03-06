/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ThumbnailDelegate.h"
#include "pdfControl/imageviewmodel.h"
#include "utils/utils.h"

#include <QPainter>
#include <QDebug>
#include <QItemSelectionModel>
#include <QAbstractItemView>

#include <DGuiApplicationHelper>

ThumbnailDelegate::ThumbnailDelegate(QAbstractItemView* parent)
    : DStyledItemDelegate(parent)
{
    m_parent = parent;
}

void ThumbnailDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    if (index.isValid()){
        qreal pixscale = m_parent->property("adaptScale").toDouble();
        int rotate = index.data(ImageinfoType_e::IMAGE_ROTATE).toInt();
        bool bShowBookMark = index.data(ImageinfoType_e::IMAGE_BOOKMARK).toBool();
        QMatrix matrix;
        matrix.rotate(rotate);
        const QPixmap& pixmap = index.data(ImageinfoType_e::IMAGE_PIXMAP).value<QPixmap>().transformed(matrix);

        if(!pixmap.isNull()){
            const int borderRadius = 6;

            const QPixmap& scalePix = pixmap.scaled(pixmap.width() * pixscale, pixmap.height() * pixscale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            const QSize& scalePixSize = scalePix.size();
            const QRect& rect = QRect(option.rect.center().x() - scalePixSize.width() / 2, option.rect.center().y() - scalePixSize.height() / 2, scalePixSize.width(), scalePixSize.height());

            //clipPath pixmap
            painter->save();
            QPainterPath clipPath;
            clipPath.addRoundedRect(rect, borderRadius, borderRadius);
            painter->setClipPath(clipPath);
            painter->drawPixmap(rect.x(), rect.y(), scalePix);
            painter->restore();
            //drawText RoundRect
            painter->save();
            painter->setBrush(Qt::NoBrush);
            painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            if(m_parent->selectionModel()->isRowSelected(index.row(), index.parent())){
                painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color(), 2));
                painter->drawRoundedRect(rect, borderRadius, borderRadius);
            }
            else{
                painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color(), 1));
                painter->drawRoundedRect(rect, borderRadius, borderRadius);
                painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().windowText().color()));
            }
            painter->drawText(rect.x(), rect.bottom() + 4, rect.width(), option.rect.bottom() - rect.bottom(), Qt::AlignHCenter | Qt::AlignTop, QString::number(index.row() + 1));
            painter->restore();
            //drawMark
            drawBookMark(painter, rect, bShowBookMark);
        }
    }
}

QSize ThumbnailDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return DStyledItemDelegate::sizeHint(option, index);
}

void ThumbnailDelegate::drawBookMark(QPainter* painter, const QRect& rect, bool visible) const
{
    if (visible){
        QString ssPath = ":/resources/image/";
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType)
            ssPath += "light";
        else if (themeType == DGuiApplicationHelper::DarkType)
            ssPath += "dark";

        ssPath += "/checked/bookmarkbig_checked_light.svg";
        QPixmap pixmap(Utils::renderSVG(ssPath, QSize(36, 36)));
        painter->drawPixmap(rect.right() - 42, rect.y(), pixmap);
    }
}
