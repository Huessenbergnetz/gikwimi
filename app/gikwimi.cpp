/*
 * SPDX-FileCopyrightText: (C) 2022, 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "logging.h"
#include "gikwimi.h"
#include "gikwimiconfig.h"
#include "confignames.h"
#include "userauthstoresql.h"
#include "global.h"

#include "controllers/root.h"
#include "controllers/login.h"
#include "controllers/logout.h"
#include "controllers/controlcenter.h"
#include "controllers/controlcentersettings.h"
#include "controllers/controlcenterusers.h"
#include "controllers/controlcenterusersettings.h"
#include "controllers/controlcenterevents.h"
#include "controllers/controlcenteraddressbooks.h"
#include "controllers/invitation.h"

#include "objects/menuitem.h"

#include <Cutelyst/Engine>
#include <Cutelyst/Plugins/StaticSimple>
#include <Cutelyst/Plugins/View/Cutelee/cuteleeview.h>
#include <Cutelyst/Plugins/Session/Session>
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/Authentication/authenticationrealm.h>
#include <Cutelyst/Plugins/Utils/Sql>
#include <Cutelyst/Plugins/StatusMessage>
#include <Cutelyst/Plugins/Memcached/Memcached>
#include <Cutelyst/Plugins/MemcachedSessionStore/MemcachedSessionStore>
#include <Cutelyst/Plugins/CSRFProtection>
#include <Cutelyst/Plugins/Utils/LangSelect>

#include <cutelee/engine.h>

#include <KContacts/Picture>

#include <QCoreApplication>
#include <QMutexLocker>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#if defined(QT_DEBUG)
Q_LOGGING_CATEGORY(GIK_CORE, "gikwimi.core")
#else
Q_LOGGING_CATEGORY(GIK_CORE, "gikwimi.core", QtInfoMsg)
#endif

using namespace Cutelyst;
using namespace Qt::StringLiterals;

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
    const auto supportedLocales = loadTranslationsFromDir(QStringLiteral("gikwimiapp"), QStringLiteral(GIKWIMI_TRANSLATIONSDIR), QStringLiteral("_"));
    qCInfo(GIK_CORE) << "Loaded locales:" << supportedLocales;

    qCDebug(GIK_CORE) << "Registering meta types";
    qRegisterMetaType<dbid_t>("dbid_t");
    qRegisterMetaType<KContacts::Picture>("KContacts::Picture");

    if (!isInitialized) {
        GikwimiConfig::load(engine()->config(QStringLiteral(GIKWIMI_CONF_GIK)),
                            engine()->config(QStringLiteral(GIKWIMI_CONF_MAIL)));

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
    new Login(this);
    new Logout(this);
    new ControlCenter(this);
    new ControlCenterSettings(this);
    new ControlCenterUsers(this);
    new ControlCenterUserSettings(this);
    new ControlCenterEvents(this);
    new ControlCenterAddressBooks(this);
    new Invitation(this);

    qCDebug(GIK_CORE) << "Registering plugins";
    auto staticSimple = new StaticSimple(this);
    staticSimple->setIncludePaths({GikwimiConfig::tmplPath(QStringLiteral("static")), QStringLiteral(GIKWIMI_STATICSDIR)});

    auto sess = new Session(this);

    if (GikwimiConfig::useMemcached()) {
        auto memc = new Memcached(this);
        memc->setDefaultConfig({
                                   {QStringLiteral("binary_protocol"), true}
                               });
        if (GikwimiConfig::useMemcachedSession()) {
            sess->setStorage(std::make_unique<MemcachedSessionStore>(this, sess));
        }
    }

    auto lsp = new LangSelect(this, LangSelect::Session);
    lsp->setFallbackLocale(QLocale(QLocale::English));
    lsp->setSupportedLocales(supportedLocales);
    lsp->setSessionKey(QStringLiteral("lang"));

    auto csrf = new CSRFProtection(this);
    csrf->setDefaultDetachTo(QStringLiteral("/csrfDenied"));

    new StatusMessage(this);

    auto auth = new Authentication(this);
    auto userCred = std::make_shared<CredentialPassword>();
    userCred->setPasswordType(CredentialPassword::Hashed);
    auto userStore = std::make_shared<UserAuthStoreSql>();
    auth->addRealm(userStore, userCred, QStringLiteral("users"));

    defaultHeaders().setHeader("X-Frame-Options"_ba, "DENY"_ba);
    defaultHeaders().setHeader("X-Content-Type-Options"_ba, "nosniff"_ba);
    defaultHeaders().setHeader("X-XSS-Protection"_ba, "1; mode=block"_ba);
    defaultHeaders().setHeader("Referrer-Policy"_ba, "origin-when-cross-origin"_ba);
    defaultHeaders().setHeader("Content-Security-Policy"_ba, "default-src 'none'; script-src 'self'; style-src 'self'; font-src 'self'; img-src 'self' data:; connect-src 'self'; base-uri 'self'; form-action 'self'; frame-ancestors 'none'; block-all-mixed-content"_ba);

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
        QString pw = dbpass;
        for (int i = 0; i < pw.size(); ++i) {
            pw[i] = QLatin1Char('*');
        }
        qCDebug(GIK_CORE, "DB Password: %s", qUtf8Printable(pw));
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
            qCCritical(GIK_CORE) << "Can not establish database connection: failed to obtain database object";
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

    if (dbtype == "QMYSQL"_L1 || dbtype == "QMARIADB"_L1) {
        QSqlQuery q(db);
        if (Q_UNLIKELY(!q.exec(u"SET time_zone = '+00:00'"_s))) {

        }
    }

    return true;
}
