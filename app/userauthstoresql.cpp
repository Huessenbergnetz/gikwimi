/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "userauthstoresql.h"
#include "logging.h"

#include <Cutelyst/Plugins/Utils/Sql>
#include <QSqlQuery>
#include <QSqlError>

UserAuthStoreSql::UserAuthStoreSql(QObject *parent)
    : AuthenticationStore(parent)
{

}

UserAuthStoreSql::~UserAuthStoreSql() = default;

AuthenticationUser UserAuthStoreSql::findUser(Context *c, const ParamsMultiMap &userinfo)
{
    AuthenticationUser user;

    const QString username = userinfo.value(QStringLiteral("username"));

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT u.id, u.type, u.password FROM users u WHERE u.username = :username"));
    q.bindValue(QStringLiteral(":username"), username);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {
            user.setId(q.value(0));
            user.insert(QStringLiteral("username"), username);
            user.insert(QStringLiteral("type"), q.value(1));
            user.insert(QStringLiteral("password"), q.value(2));

            QSqlQuery qq = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT name, value FROM user_settings WHERE user_id = :user_id"));
            qq.bindValue(QStringLiteral(":user_id"), user.id());

            if (Q_LIKELY(qq.exec())) {
                while (Q_LIKELY(qq.next())) {
                    user.insert(qq.value(0).toString(), qq.value(1));
                }
            } else {
                qCCritical(GIK_AUTH) << "Failed to execute database query to get user settings for user" << username << "from the database:" << qq.lastError().text();
            }

        } else {
            qCWarning(GIK_AUTH) << "Can not find user" << username << "in the database";
        }
    } else {
        qCCritical(GIK_AUTH) << "Failed to execute database query to get user" << username << "from the database:" << q.lastError().text();
    }

    return user;
}

#include "moc_userauthstoresql.cpp"
