/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
#include "DBManager.h"

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QStandardPaths>

#include "application.h"

#include "subjectObserver/MsgHeader.h"
#include "utils/utils.h"

bool DBManager::hasFilePathDB(const QString &filePath)
{
    const QSqlDatabase db = getDatabase();
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query(db);
    QString sql = QString("select FilePath from FilesTable where FilePath = '%1'").arg(filePath);
//    query.prepare("select FilePath from FileFontTable where FilePath = ?");
//    query.addBindValue(filePath);
    if (query.exec(sql)) {
        while (query.next()) {
            qDebug() << "  FilePath:" << query.value(0).toString();
            if (query.value(0).toString() != "")
                return true;
        }
    }
    return false;
}

DBManager::DBManager(QObject *parent)
    : QObject(parent)
    , m_connectionName("default_connection")
{
}

const QSqlDatabase DBManager::getDatabase()
{
    QMutexLocker mutex(&m_mutex);
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        return db;
    } else {
        //if database not open, open it.
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);//not dbConnection
        QString sDbPath = Utils::getConfigPath();
        db.setDatabaseName(sDbPath + "/" + ConstantMsg::g_db_name);
        if (! db.open()) {
            qWarning() << "Open database error:" << db.lastError();
            return QSqlDatabase();
        } else {
            return db;
        }
    }
}

void DBManager::setStrFilePath(const QString &strFilePath)
{
    m_strFilePath = strFilePath;

    int nLastPos = strFilePath.lastIndexOf('/');
    nLastPos++;
    m_strFileName = strFilePath.mid(nLastPos);
}
