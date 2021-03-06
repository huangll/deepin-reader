/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "CatalogTreeView.h"

#include <QHeaderView>
#include <DFontSizeManager>
#include <QDebug>

#include "application.h"

#include "pdfControl/docview/docummentproxy.h"
#include "utils/utils.h"
#include "DocSheet.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"

#include <QProxyStyle>
#include <QPainter>
#include <QStylePainter>

class ActiveProxyStyle : public QProxyStyle
{
public:
    ActiveProxyStyle(DWidget *parent)
    {
        this->setParent(parent);
    }

    virtual void drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        QStyleOptionComplex *op = const_cast<QStyleOptionComplex *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawComplexControl(control, op, painter, widget);
    }
    virtual void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        QStyleOption *op = const_cast<QStyleOption *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawControl(element, op, painter, widget);
    }

    virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const
    {
        if (element == QStyle::PE_IndicatorBranch)
            painter->setPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().text().color());
        QStyleOption *op = const_cast<QStyleOption *>(option);
        op->state = option->state | QStyle::State_Active;
        QProxyStyle::drawPrimitive(element, op, painter, widget);
    }
};

CatalogTreeView::CatalogTreeView(DocSheet *sheet, DWidget *parent)
    : DTreeView(parent), m_sheet(sheet)
{
    ActiveProxyStyle *style = new ActiveProxyStyle(this);
    setStyle(style);
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    this->header()->setHidden(true);
    this->viewport()->setAutoFillBackground(false);
    this->setContentsMargins(0, 0, 0, 0);

    slotThemeChanged();

    connect(this, SIGNAL(collapsed(const QModelIndex &)), SLOT(SlotCollapsed(const QModelIndex &)));
    connect(this, SIGNAL(expanded(const QModelIndex &)), SLOT(SlotExpanded(const QModelIndex &)));
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClicked(QModelIndex)));
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CatalogTreeView::slotThemeChanged);
}

CatalogTreeView::~CatalogTreeView()
{

}

void CatalogTreeView::setPage(int page)
{
    m_page = page;
    this->clearSelection(); //  清除 之前的选中
//    m_listTitle.clear();
//    m_listPage.clear();

    auto model = reinterpret_cast<QStandardItemModel *>(this->model());
    if (model) {
        page++;
        auto itemList = model->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
        foreach (auto item, itemList) {
            int itemPage = item->data().toInt();

            if (itemPage == page) {    //  找到了
                auto curIndex = model->indexFromItem(item);
                if (curIndex.isValid()) {
                    auto curSelIndex = curIndex;
                    auto parentIndex = curIndex.parent();   //  父 节点
                    if (parentIndex.isValid()) {    //  父节点存在
                        auto grandpaIndex = parentIndex.parent();       //  是否还存在 爷爷节点
                        if (grandpaIndex.isValid()) {    //  爷爷节点存在
                            bool isGrandpaExpand = this->isExpanded(grandpaIndex);
                            if (isGrandpaExpand) { //  爷爷节点 已 展开
                                bool isExpand = this->isExpanded(parentIndex);
                                if (!isExpand) {   //  父节点未展开, 则 父节点高亮
                                    curSelIndex = parentIndex;
                                }
                            } else {    //  爷爷节点 未展开, 则 爷爷节点高亮
                                curSelIndex = grandpaIndex;
                            }
                        } else {    //  没有 爷爷节点
                            bool isExpand = this->isExpanded(parentIndex);
                            if (!isExpand)     //  父节点未展开, 则 父节点高亮
                                curSelIndex = parentIndex;
                        }
                    }
                    this->selectionModel()->clearSelection();
                    this->selectionModel()->select(curSelIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                }
                break;
            }
        }
    }
}

void CatalogTreeView::setRightControl(bool hasControl)
{
    rightnotifypagechanged = hasControl;
}

//  递归解析
void CatalogTreeView::parseCatalogData(const Section &ol, QStandardItem *parentItem)
{
    auto model = reinterpret_cast<QStandardItemModel *>(this->model());
    if (model) {
        foreach (auto s, ol.children) { //  2级显示
            if (s.link.page > 0) {
                auto itemList = getItemList(s.title, s.link.page, s.link.left, s.link.top);
                parentItem->appendRow(itemList);

                foreach (auto s1, s.children) { //  3级显示
                    if (s1.link.page > 0) {
                        auto itemList1 = getItemList(s1.title, s1.link.page, s1.link.left, s1.link.top);
                        itemList.at(0)->appendRow(itemList1);
                    }
                }
            }
        }
    }
}

//  获取 一行的 三列数据
QList<QStandardItem *> CatalogTreeView::getItemList(const QString &title, const int &page, const qreal  &realleft, const qreal &realtop)
{
//    QColor color = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().text().color();
    auto item = new QStandardItem(title);
    item->setData(page);
    item->setData(realleft, Qt::UserRole + 2);
    item->setData(realtop, Qt::UserRole + 3);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//    item->setForeground(QBrush(color));
//    m_listTitle.append(item);

    auto item1 = new QStandardItem(QString::number(page));
    item1->setData(page);
    item1->setData(realleft, Qt::UserRole + 2);
    item1->setData(realtop, Qt::UserRole + 3);
    item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QColor color = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().textTips().color();
    item1->setForeground(QBrush(color));
//    m_listPage.append(item1);

    return QList<QStandardItem *>() << item << item1;
}

//  文档打开成功, 加载对应目录
void CatalogTreeView::handleOpenSuccess()
{
    auto model = reinterpret_cast<QStandardItemModel *>(this->model());
    if (model) {
        model->clear();

        if (nullptr == m_sheet)
            return;

        DocummentProxy *_proxy =  m_sheet->getDocProxy();
        if (_proxy) {

            Outline ol = _proxy->outline();

            foreach (const Section &s, ol) {   //  1 级显示
                if (s.link.page > 0) {
                    auto itemList = getItemList(s.title, s.link.page, s.link.left, s.link.top);
                    model->appendRow(itemList);

                    parseCatalogData(s, itemList.at(0));
                }
            }

            int nCurPage = _proxy->currentPageNo();
            setPage(nCurPage);
        }
    }
}

//  折叠 节点处理
void CatalogTreeView::SlotCollapsed(const QModelIndex &index)
{
    Q_UNUSED(index);

    if (nullptr == m_sheet)
        return;

    setPage(m_page);
}

//  展开 节点处理
void CatalogTreeView::SlotExpanded(const QModelIndex &index)
{
    Q_UNUSED(index);

    if (nullptr == m_sheet)
        return;

    setPage(m_page);
}

//  实现 上下左键 跳转
void CatalogTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (!rightnotifypagechanged) {

        if (nullptr == m_sheet)
            return;

        DocummentProxy *_proxy =  m_sheet->getDocProxy();

        if (_proxy) {
            int nPage = current.data(Qt::UserRole + 1).toInt();

            nPage--;

            double left = current.data(Qt::UserRole + 2).toDouble();

            double top = current.data(Qt::UserRole + 3).toDouble();

            _proxy->jumpToOutline(left, top, nPage);

        }
    }
    rightnotifypagechanged = false;

    return DTreeView::currentChanged(current, previous);
}

void CatalogTreeView::onItemClicked(const QModelIndex &current)
{
    if (nullptr == m_sheet)
        return;

    DocummentProxy *_proxy =  m_sheet->getDocProxy();

    if (_proxy) {
        int nPage = current.data(Qt::UserRole + 1).toInt();

        nPage--;

        double left = current.data(Qt::UserRole + 2).toDouble();

        double top = current.data(Qt::UserRole + 3).toDouble();

        _proxy->jumpToOutline(left, top, nPage);

    }
}

void CatalogTreeView::slotThemeChanged()
{
//    auto color = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().text().color();
//    for (int index = 0; index < m_listTitle.count(); index++) {
//        QStandardItem *item = m_listTitle.at(index);
//        if (item) {
//            item->setData(QVariant::fromValue(color), Qt::ForegroundRole);
//            item->setForeground(QBrush(color));
//        }
//    }

//    color = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().textTips().color();
//    for (int index = 0; index < m_listPage.count(); index++) {
//        QStandardItem *item = m_listPage.at(index);
//        if (item) {
//            item->setData(QVariant::fromValue(color), Qt::ForegroundRole);
//            item->setForeground(QBrush(color));
//        }
//    }

//    this->update();
}

//  窗口大小变化, 列的宽度随之变化
void CatalogTreeView::resizeEvent(QResizeEvent *event)
{
    setColumnWidth(0, this->width() - 60);
    setColumnWidth(1, 50);

    DTreeView::resizeEvent(event);
}

void CatalogTreeView::mousePressEvent(QMouseEvent *event)
{
    rightnotifypagechanged = false;
    DTreeView::mousePressEvent(event);
}

void CatalogTreeView::keyPressEvent(QKeyEvent *event)
{
    rightnotifypagechanged = false;
    DTreeView::keyPressEvent(event);
}
