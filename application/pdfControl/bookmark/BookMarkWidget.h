/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     leiyu
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
#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <QPointer>

#include "CustomControl/CustomWidget.h"

class DocSheet;
class ImageListView;
class BookMarkWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BookMarkWidget)

signals:
    void sigSetBookMarkState(const int &, const int &);

public:
    explicit BookMarkWidget(DocSheet *sheet, DWidget *parent = nullptr);
    ~BookMarkWidget() override;

    void prevPage();
    void nextPage();
    void DeleteItemByKey();

    void handlePage(int page);
    void handleRotate(int rotate);
    void handleOpenSuccess();
    void handleBookMark(int page, int state);

public:
    void initWidget() Q_DECL_OVERRIDE;
    void adaptWindowSize(const double &) Q_DECL_OVERRIDE;
    void updateThumbnail(const int &) Q_DECL_OVERRIDE;

private slots:
    void onUpdateTheme();
    void onAddBookMarkClicked();
    void onListMenuClick(const int &iType);

private:
    void scrollToCurrentPage();

private:
    QPointer<DocSheet>   m_sheet;
    DPushButton         *m_pAddBookMarkBtn;
    ImageListView       *m_pImageListView;
};

#endif  // BOOKMARKFORM_H
