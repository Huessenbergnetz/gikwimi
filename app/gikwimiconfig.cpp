/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "gikwimiconfig.h"
#include "confignames.h"

#include <QGlobalStatic>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

struct ConfigValues {
    mutable QReadWriteLock lock{QReadWriteLock::Recursive};

    QString tmpl = QStringLiteral(GIKWIMI_CONF_GIK_TEMPLATE_DEFVAL);
    QString tmplDir = QStringLiteral(GIKWIMI_TEMPLATEDIR);
};
Q_GLOBAL_STATIC(ConfigValues, cfg)

void GikwimiConfig::load(const QVariantMap &gikwimi)
{
    QWriteLocker locker(&cfg->lock);

    cfg->tmpl = gikwimi.value(QStringLiteral(GIKWIMI_CONF_GIK_TEMPLATE), QStringLiteral(GIKWIMI_CONF_GIK_TEMPLATE_DEFVAL)).toString();
    if (cfg->tmpl.startsWith(QLatin1Char('/'))) {
        QStringList fullPathParts = cfg->tmpl.split(QLatin1Char('/'), Qt::SkipEmptyParts);
        cfg->tmpl = fullPathParts.takeLast();
        cfg->tmplDir = QLatin1Char('/') + fullPathParts.join(QLatin1Char('/'));
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
