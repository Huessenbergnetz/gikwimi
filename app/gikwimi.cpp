/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "logging.h"
#include "gikwimi.h"
#include "gikwimiconfig.h"
#include "root.h"
#include "confignames.h"

#include <Cutelyst/Engine>
#include <Cutelyst/Plugins/StaticSimple/StaticSimple>
#include <Cutelyst/Plugins/View/Cutelee/cuteleeview.h>
#include <Cutelyst/Plugins/Session/Session>
#include <Cutelyst/Plugins/Utils/Sql>
#include <Cutelyst/Plugins/StatusMessage>
#include <Cutelyst/Plugins/Memcached/Memcached>
#include <Cutelyst/Plugins/MemcachedSessionStore/MemcachedSessionStore>

#include <cutelee/engine.h>

#include <QCoreApplication>
#include <QMutexLocker>
#include <QSqlDatabase>
#include <QSqlError>

Q_LOGGING_CATEGORY(GIK_CORE, "gikwimi.core")

using namespace Cutelyst;

static QMutex mutex; // clazy:exclude=non-pod-global-static

bool Gikwimi::isInitialized = false;
bool Gikwimi::messageHandlerInstalled = false;

Gikwimi::Gikwimi(QObject *parent) : Application(parent)
{
    QCoreApplication::setApplicationName(QStringLiteral("Gikwimi"));
    QCoreApplication::setApplicationVersion(QStringLiteral(GIKWIMI_VERSION));
}

Gikwimi::~Gikwimi()
{
}

bool Gikwimi::init()
{
    if (!isInitialized) {
        GikwimiConfig::load(engine()->config(QStringLiteral(GIKWIMI_CONF_GIK)));

        qCInfo(GIK_CORE) << "Template:" << GikwimiConfig::tmplPath();

        // initialize DB one time to prevent https://bugreports.qt.io/browse/QTBUG-54872
        if (!initDb()) {
            return false;
        }

        QSqlDatabase::removeDatabase(Sql::databaseNameThread());

        isInitialized = true;
    }

    qCDebug(GIK_CORE) << "Registering Cutelee view";
    auto view = new CuteleeView(this);
    view->setTemplateExtension(QStringLiteral(".tmpl"));
    view->setWrapper(QStringLiteral("wrapper.tmpl"));
    view->setCache(false);
    view->setIncludePaths({GikwimiConfig::tmplPath(QStringLiteral("site"))});
    qCDebug(GIK_CORE) << "Cutelee view include paths:" << view->includePaths();
    view->engine()->addDefaultLibrary(QStringLiteral("cutelee_i18ntags"));

    qCDebug(GIK_CORE) << "Registering controllers";
    new Root(this);

    qCDebug(GIK_CORE) << "Registering plugins";
    auto staticSimple = new StaticSimple(this);
    staticSimple->setIncludePaths({GikwimiConfig::tmplPath(QStringLiteral("static"))});

    auto sess = new Session(this);

    new StatusMessage(this);

    return true;
}

bool Gikwimi::postFork()
{
    QMutexLocker locker(&mutex);

    return initDb();
}

bool Gikwimi::initDb()
{
    const QVariantMap dbconfig = engine()->config(QStringLiteral(GIKWIMI_CONF_DB));
    const QString dbtype = dbconfig.value(QStringLiteral(GIKWIMI_CONF_DB_TYPE), QStringLiteral(GIKWIMI_CONF_DB_TYPE_DEFVAL)).toString();
    const QString dbname = dbconfig.value(QStringLiteral(GIKWIMI_CONF_DB_NAME), QStringLiteral(GIKWIMI_CONF_DB_NAME_DEFVAL)).toString();
    const QString dbuser = dbconfig.value(QStringLiteral(GIKWIMI_CONF_DB_USER), QStringLiteral(GIKWIMI_CONF_DB_USER_DEFVAL)).toString();
    const QString dbpass = dbconfig.value(QStringLiteral(GIKWIMI_CONF_DB_PASS)).toString();
    const QString dbhost = dbconfig.value(QStringLiteral(GIKWIMI_CONF_DB_HOST), QStringLiteral(GIKWIMI_CONF_DB_HOST_DEFVAL)).toString();
    const int dbport = dbconfig.value(QStringLiteral(GIKWIMI_CONF_DB_PORT), GIKWIMI_CONF_DB_PORT_DEFVAL).toInt();


    if (!isInitialized) {
        qCDebug(GIK_CORE, "Initializing database connection");
        qCDebug(GIK_CORE, "DB Type: %s", qUtf8Printable(dbtype));
        qCDebug(GIK_CORE, "DB Name: %s", qUtf8Printable(dbname));
        qCDebug(GIK_CORE, "DB User: %s", qUtf8Printable(dbuser));
        qCDebug(GIK_CORE, "DB Host: %s", qUtf8Printable(dbhost));
        qCDebug(GIK_CORE, "DB Port: %i", dbport);
    } else {
        qCDebug(GIK_CORE) << "Establishing database connection";
    }

    QSqlDatabase db;
    const QString dbConName = Sql::databaseNameThread();
    if (dbtype == QLatin1String("QMYSQL")) {
        if (Q_UNLIKELY(dbhost.isEmpty())) {
            qCCritical(GIK_CORE) << "Can not establish database connection: missing database hostname";
            return false;
        }
        if (Q_UNLIKELY(dbname.isEmpty())) {
            qCCritical(GIK_CORE) << "Can not establish database connection: missing database name";
            return false;
        }
        if (Q_UNLIKELY(dbuser.isEmpty())) {
            qCCritical(GIK_CORE) << "Can not establish database connection: missing user name";
            return false;
        }

        db = QSqlDatabase::addDatabase(dbtype, dbConName);
        if (Q_LIKELY(db.isValid())) {
            db.setDatabaseName(dbname);
            db.setUserName(dbuser);
            if (!dbpass.isEmpty()) {
                db.setPassword(dbpass);
            }

            if (dbhost[0] == QLatin1Char('/')) {
                db.setConnectOptions(QStringLiteral("UNIX_SOCKET=%1;MYSQL_OPT_RECONNECT=1;CLIENT_INTERACTIVE=1").arg(dbhost));
            } else {
                db.setConnectOptions(QStringLiteral("MYSQL_OPT_RECONNECT=1;CLIENT_INTERACTIVE=1"));
                db.setHostName(dbhost);
                db.setPort(dbport);
            }

        } else {
            qCCritical(GIK_CORE) << "Can not establsih database connection: failed to obtain database object";
            return false;
        }
    } else {
        qCCritical(GIK_CORE) << "Can not establish database connection:" << dbtype << "is not a supported database type";
        return false;
    }

    if (Q_UNLIKELY(!db.open())) {
        qCCritical(GIK_CORE) << "Can not establish database connection:" << db.lastError().text();
        return false;
    }

    return true;
}
