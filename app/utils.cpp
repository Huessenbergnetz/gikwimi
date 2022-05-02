/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "utils.h"
#include "objects/user.h"
#include "objects/error.h"

#include <Cutelyst/Context>

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
