/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "gikwimiconfig.h"
#include "confignames.h"
#include "logging.h"

#include <Cutelyst/Plugins/Utils/Sql>
#include <Cutelyst/Plugins/Memcached/Memcached>

#include <QGlobalStatic>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QSqlQuery>
#include <QSqlError>

#define MEMC_CONFIG_GROUP_KEY "options"
#define MEMC_CONFIG_STORAGE_DURATION 7200

Q_LOGGING_CATEGORY(GIK_CONFIG, "gikwimi.config")

struct ConfigValues {
    mutable QReadWriteLock lock{QReadWriteLock::Recursive};

    QString tmpl = QStringLiteral(GIKWIMI_CONF_GIK_TEMPLATE_DEFVAL);
    QString tmplDir = QStringLiteral(GIKWIMI_TEMPLATESDIR);
    QString siteName = QStringLiteral(GIKWIMI_CONF_GIK_SITENAME_DEFVAL);
    QString emailHost;
    QString emailFromName;
    QString emailFromMail = QStringLiteral(GIKWIMI_CONF_MAIL_FROM_NAME_DEFVAL);
    QString emailUser;
    QString emailPassword;
    int emailPort = GIKWIMI_CONF_MAIL_PORT_DEFVAL;
    SimpleMail::Server::ConnectionType emailConnectionType = SimpleMail::Server::SslConnection;
    bool useMemcached = GIKWIMI_CONF_GIK_USEMEMCACHED_DEFVAL;
    bool useMemcachedSession = GIKWIMI_CONF_GIK_USEMEMCACHEDSESSION_DEFVAL;
};
Q_GLOBAL_STATIC(ConfigValues, cfg)

void GikwimiConfig::load(const QVariantMap &gikwimi, const QVariantMap &email)
{
    QWriteLocker locker(&cfg->lock);

    cfg->tmpl = gikwimi.value(QStringLiteral(GIKWIMI_CONF_GIK_TEMPLATE), QStringLiteral(GIKWIMI_CONF_GIK_TEMPLATE_DEFVAL)).toString();
    if (cfg->tmpl.startsWith(QLatin1Char('/'))) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QStringList fullPathParts = cfg->tmpl.split(QLatin1Char('/'), Qt::SkipEmptyParts);
#else
        QStringList fullPathParts = cfg->tmpl.split(QLatin1Char('/'), QString::SkipEmptyParts);
#endif
        cfg->tmpl = fullPathParts.takeLast();
        cfg->tmplDir = QLatin1Char('/') + fullPathParts.join(QLatin1Char('/'));
    }

    cfg->siteName = gikwimi.value(QStringLiteral(GIKWIMI_CONF_GIK_SITENAME), QStringLiteral(GIKWIMI_CONF_GIK_SITENAME_DEFVAL)).toString();

    cfg->useMemcached = gikwimi.value(QStringLiteral(GIKWIMI_CONF_GIK_USEMEMCACHED), GIKWIMI_CONF_GIK_USEMEMCACHED_DEFVAL).toBool();
    cfg->useMemcachedSession = gikwimi.value(QStringLiteral(GIKWIMI_CONF_GIK_USEMEMCACHEDSESSION), GIKWIMI_CONF_GIK_USEMEMCACHEDSESSION_DEFVAL).toBool();

    cfg->emailHost = email.value(QStringLiteral(GIKWIMI_CONF_MAIL_HOST)).toString();
    cfg->emailPort = email.value(QStringLiteral(GIKWIMI_CONF_MAIL_PORT), GIKWIMI_CONF_MAIL_PORT_DEFVAL).toInt();
    cfg->emailFromMail = email.value(QStringLiteral(GIKWIMI_CONF_MAIL_FROM_MAIL)).toString();
    cfg->emailFromName = email.value(QStringLiteral(GIKWIMI_CONF_MAIL_FROM_NAME), QStringLiteral(GIKWIMI_CONF_MAIL_FROM_NAME_DEFVAL)).toString();
    cfg->emailUser = email.value(QStringLiteral(GIKWIMI_CONF_MAIL_USER)).toString();
    cfg->emailPassword = email.value(QStringLiteral(GIKWIMI_CONF_MAIL_PASS)).toString();

    const QString enc = email.value(QStringLiteral(GIKWIMI_CONF_MAIL_ENC), QStringLiteral(GIKWIMI_CONF_MAIL_ENC_DEFVAL)).toString();
    if (enc.compare(QLatin1String("ssl"), Qt::CaseInsensitive) == 0 || enc.compare(QLatin1String("tls"), Qt::CaseInsensitive) == 0) {
        cfg->emailConnectionType = SimpleMail::Server::SslConnection;
    } else if (enc.compare(QLatin1String("starttls"), Qt::CaseInsensitive) == 0) {
        cfg->emailConnectionType = SimpleMail::Server::TlsConnection;
    } else {
        cfg->emailConnectionType = SimpleMail::Server::TcpConnection;
    }
}

QString GikwimiConfig::tmpl()
{
    QReadLocker locker(&cfg->lock);
    return cfg->tmpl;
}

QString GikwimiConfig::tmplPath()
{
    QReadLocker locker(&cfg->lock);
    return cfg->tmplDir + QLatin1Char('/') + cfg->tmpl;
}

QString GikwimiConfig::tmplPath(const QString &path)
{
    return GikwimiConfig::tmplPath() + QLatin1Char('/') + path;
}

QString GikwimiConfig::tmplPath(const QStringList &pathParts)
{
    return tmplPath(pathParts.join(QLatin1Char('/')));
}

QString GikwimiConfig::siteName()
{
    QReadLocker locker(&cfg->lock);
    return cfg->siteName;
}

bool GikwimiConfig::useMemcached()
{
    QReadLocker locker(&cfg->lock);
    return cfg->useMemcached;
}

bool GikwimiConfig::useMemcachedSession()
{
    QReadLocker locker(&cfg->lock);
    return cfg->useMemcachedSession;
}

QString GikwimiConfig::emailHost()
{
    QReadLocker locker(&cfg->lock);
    return cfg->emailHost;
}

int GikwimiConfig::emailPort()
{
    QReadLocker locker(&cfg->lock);
    return cfg->emailPort;
}

QString GikwimiConfig::emailFromMail()
{
    QReadLocker locker(&cfg->lock);
    return cfg->emailFromMail;
}

QString GikwimiConfig::emailFromName()
{
    QReadLocker locker(&cfg->lock);
    return cfg->emailFromName;
}

QString GikwimiConfig::emailUser()
{
    QReadLocker locker(&cfg->lock);
    return cfg->emailUser;
}

QString GikwimiConfig::emailPassword()
{
    QReadLocker locker(&cfg->lock);
    return cfg->emailPassword;
}

SimpleMail::Server::ConnectionType GikwimiConfig::emailConnectionType()
{
    QReadLocker locker(&cfg->lock);
    return cfg->emailConnectionType;
}

template< typename T >
T GikwimiConfig::getDbOption(const QString &option, const T &defVal)
{
    T retVal = defVal;

    if (cfg->useMemcached) {
        Cutelyst::Memcached::MemcachedReturnType rt;
        T val = Cutelyst::Memcached::getByKey<T>(QStringLiteral(MEMC_CONFIG_GROUP_KEY), option, nullptr, &rt);
        if (rt == Cutelyst::Memcached::Success) {
            return val;
        }
    }

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT option_value FROM options WHERE option_name = :option_name"));
    q.bindValue(QStringLiteral(":option_name"), option);

    if (Q_LIKELY(q.exec())) {
        if (q.next()) {
            retVal = q.value(0).value<T>();
        }
    } else {
        qCCritical(GIK_CONFIG) << "Failed to query option" << option << "from database:" << q.lastError().text();
    }

    if (cfg->useMemcached) {
        Cutelyst::Memcached::setByKey<T>(QStringLiteral(MEMC_CONFIG_GROUP_KEY), option, retVal, MEMC_CONFIG_STORAGE_DURATION);
    }

    return retVal;
}

template< typename T >
bool GikwimiConfig::setDbOption(const QString &option, const T &value)
{
    bool rv = false;

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("INSERT INTO options (option_name, option_value) "
                                                         "VALUES (:option_Name, :option_value) "
                                                         "ON DUPLICATE KEY UPDATE "
                                                         "option_value = :option_value"));
    q.bindValue(QStringLiteral(":option_name"), option);
    q.bindValue(QStringLiteral(":option_value"), QVariant::fromValue<T>(value));


    rv = q.exec();

    if (Q_UNLIKELY(!rv)) {
        qCCritical(GIK_CONFIG) << "Failed to save value" << value << "for option" << option << "in database:" << q.lastError().text();
    }

    if (rv && cfg->useMemcached) {
        Cutelyst::Memcached::setByKey<T>(QStringLiteral(MEMC_CONFIG_GROUP_KEY), option, value, MEMC_CONFIG_STORAGE_DURATION);
    }

    return rv;
}
