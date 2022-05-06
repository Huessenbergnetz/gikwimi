/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "utils.h"
#include "logging.h"
#include "objects/user.h"
#include "objects/error.h"

#include <Cutelyst/Context>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

#include <limits>

bool Utils::isCurrentUserAdmin(Cutelyst::Context *c)
{
    return User::fromStash(c).isAdmin();
}

dbid_t Utils::strToDbid(const QString &str, bool *ok, const QString &errorMsg, Cutelyst::Context *c)
{
    dbid_t val = 0;

    Q_ASSERT_X(!errorMsg.isEmpty() && c, "string to dbid_t", "invalid context object");

    bool _ok = false;
    const qulonglong tmpval = str.toULongLong(&_ok);
    if (Q_LIKELY(_ok)) {
        if (Q_LIKELY(tmpval >= static_cast<qulonglong>(std::numeric_limits<dbid_t>::min() && tmpval <= static_cast<qulonglong>(std::numeric_limits<dbid_t>::max())))) {
            val = static_cast<dbid_t>(tmpval);
        } else {
            _ok = false;
        }
    }

    if (ok) {
        *ok = _ok;
    }

    if (!_ok && !errorMsg.isEmpty() && c) {
        Error::toStash(c, Error::InputError, errorMsg);
    }

    return val;
}

QVariantHash Utils::settingsHashFromString(const QString &str)
{
    QVariantHash hash;

    QJsonParseError jsonError;
    const QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    if (jsonError.error == QJsonParseError::NoError) {
        if (!doc.isEmpty()) {
            const QJsonObject obj = doc.object();
            hash = obj.toVariantHash();
        }
    } else {
        qCCritical(GIK_CORE) << "Failed to parse JSON settings:" << jsonError.error << jsonError.errorString();
    }

    return hash;
}

QString Utils::settingsHashToString(const QVariantHash &hash)
{
    QString str;

    const QJsonObject obj = QJsonObject::fromVariantHash(hash);
    const QJsonDocument doc(obj);
    str = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    return str;
}

QString Utils::createSlug(const QString &str)
{
    QString slug;

    for (const QChar &ch : str) {
        if (ch.isSpace()) {
            slug.append(QLatin1Char('-'));
        } else if (ch.isPunct()) {
            continue;
        } else {
            const char c = ch.toLatin1();
            if (c != 0) {
                slug.append(QChar::fromLatin1(c).toLower());
            }
        }
    }

    return slug;
}
