/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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
#ifndef SHORTCUTSHOW_H
#define SHORTCUTSHOW_H

#include <QObject>
#include <QMap>

/**
 * @brief The ShortCutShow class
 * @brief   显示快捷键预览
 */

class ShortCutShow : public QObject
{
    Q_OBJECT
public:
    explicit ShortCutShow(QObject *parent = nullptr);

public:
    void show();

private:
    void initData();

private:
    QStringList Settingsnames, Filesnames, Displaynames, Toolsnames, Editnames,  shortcutnames, windowKeymaps;
    QMap<QString, QString> shortcutmap;
};

#endif // SHORTCUTSHOW_H
