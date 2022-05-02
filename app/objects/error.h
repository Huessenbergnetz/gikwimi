/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_ERROR_H
#define GIKWIMI_ERROR_H

#include <Cutelyst/Response>

#include <QObject>
#include <QSharedDataPointer>

class ErrorData;
class QSqlError;

namespace Cutelyst {
class Context;
}

class Error
{
    Q_GADGET
public:
    enum Type : quint8 {
        NoError,
        SqlError,
        ConfigError,
        ApplicationError,
        AuthenticationError,
        AuthorizationError,
        NotFound,
        InputError,
        UnknownError
    };
    Q_ENUM(Type)

    Error();

    Error(Error::Type tye, const QString &text);

    Error(const QSqlError &sqlError, const QString &text);

    Error(const Error &other);

    Error(Error &&other) noexcept;

    Error &operator=(const Error &other);

    Error &operator=(Error &&other) noexcept;

    ~Error();

    Error::Type type() const;

    Cutelyst::Response::HttpStatus status() const;

    QString text() const;

    QString sqlErrorText() const;

    QString title(Cutelyst::Context *c) const;

    void toStash(Cutelyst::Context *c, bool detach = false) const;

    static Error fromStash(Cutelyst::Context *c);

    static void toStash(Cutelyst::Context *c, Error::Type type, const QString &errorText = QString(), bool detach = true);

private:
    QSharedDataPointer<ErrorData> d;
};

#endif // GIKWIMI_ERROR_H
