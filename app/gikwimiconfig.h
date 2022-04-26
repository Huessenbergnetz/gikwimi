/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICONFIG_H
#define GIKWIMICONFIG_H

#include <QString>
#include <QStringList>
#include <QVariantMap>

class GikwimiConfig
{
public:
    static void load(const QVariantMap &gikwimi);

    static QString tmpl();
    static QString tmplPath();
    static QString tmplPath(const QString &path);
    static QString tmplPath(const QStringList &pathParts);

    static QString siteName();
    static bool useMemcached();
    static bool useMemcachedSession();

private:
    template< typename T >
    static T getDbOption(const QString &option, const T &defVal);
    template< typename T >
    static bool setDbOption(const QString &option, const T &value);
};

#endif // GIKWIMICONFIG_H
