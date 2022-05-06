/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_UTILS_H
#define GIKWIMI_UTILS_H

#include "global.h"

#include <QString>
#include <QVariantHash>

namespace Cutelyst {
class Context;
}

class Utils
{
public:
    static bool isCurrentUserAdmin(Cutelyst::Context *c);

    static dbid_t strToDbid(const QString &str, bool *ok = nullptr, const QString &errorMsg = QString(), Cutelyst::Context *c = nullptr);

    static QVariantHash settingsHashFromString(const QString &str);
    static QString settingsHashToString(const QVariantHash &hash);

    static QString createSlug(const QString &str);
};

#endif // GIKWIMI_UTILS_H
