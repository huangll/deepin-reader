/* Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
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
#include "CustomWidgetPrivate.h"

CustomWidgetPrivate::CustomWidgetPrivate(CustomWidget *cw)
    : q_ptr(cw)
{

}

QList<int> CustomWidgetPrivate::getMsgList() const
{
    return m_pMsgList;
}

QList<QString> CustomWidgetPrivate::getKeyMsgList() const
{
    return m_pKeyMsgList;
}

QString CustomWidgetPrivate::getBindPath() const
{
    return m_strBindPath;
}

void CustomWidgetPrivate::setBindPath(const QString &strBindPath)
{
    m_strBindPath = strBindPath;
}
