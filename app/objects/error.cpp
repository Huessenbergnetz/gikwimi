/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "error.h"

#include <Cutelyst/Context>

#include <QSqlError>

#define STASH_KEY "_gik_error"

class ErrorData : public QSharedData
{
public:
    ErrorData() : QSharedData() {}

    ErrorData(Error::Type _type, const QString &_text)
        : QSharedData(),
          text{_text},
          type{_type}
    {
        switch(type) {
        case Error::AuthenticationError:
            status = Cutelyst::Response::Unauthorized;
            break;
        case Error::AuthorizationError:
            status = Cutelyst::Response::Forbidden;
            break;
        case Error::NotFound:
            status = Cutelyst::Response::NotFound;
            break;
        case Error::InputError:
            status = Cutelyst::Response::BadRequest;
            break;
        default:
            status = Cutelyst::Response::InternalServerError;
            break;
        }
    }

    ErrorData(const QSqlError &_sqlError, const QString &_text)
        : QSharedData(),
          text{_text},
          sqlError{_sqlError},
          type{Error::SqlError},
          status{Cutelyst::Response::InternalServerError}
    {

    }

    QString text;
    QSqlError sqlError;
    Error::Type type = Error::NoError;
    Cutelyst::Response::HttpStatus status = Cutelyst::Response::OK;
};

Error::Error()
    : d(new ErrorData)
{

}

Error::Error(Error::Type type, const QString &text)
    : d(new ErrorData(type, text))
{

}

Error::Error(const QSqlError &sqlError, const QString &text)
    : d(new ErrorData(sqlError, text))
{

}

Error::Error(const Error &other) = default;

Error::Error(Error &&oher) noexcept = default;

Error &Error::operator=(const Error &other) = default;

Error &Error::operator=(Error &&other) noexcept = default;

Error::~Error() = default;

Error::Type Error::type() const
{
    return d->type;
}

Cutelyst::Response::HttpStatus Error::status() const
{
    return d->status;
}

QString Error::text() const
{
    return d->text;
}

QString Error::sqlErrorText() const
{
    return d->sqlError.text();
}

QString Error::title(Cutelyst::Context *c) const
{
    switch(d->type) {
    case NoError:
        return c->translate("Error", "No error");
    case SqlError:
        return c->translate("Error", "Database error");
    case ConfigError:
        return c->translate("Error", "Configuration error");
    case ApplicationError:
        return c->translate("Error", "Internet server error");
    case AuthenticationError:
    case AuthorizationError:
        return c->translate("Error", "Access denied");
    case NotFound:
        return c->translate("Error", "Not found");
    case InputError:
        return c->translate("Error", "Invalid input data");
    default:
        return c->translate("Error", "Unknown error");
    }
}

void Error::toStash(Cutelyst::Context *c, bool detach) const
{
    Q_ASSERT(c);
    c->setStash(QStringLiteral(STASH_KEY), QVariant::fromValue<Error>(*this));
    if (detach) {
        c->detach(c->getAction(QStringLiteral("error")));
    }
}

Error Error::fromStash(Cutelyst::Context *c)
{
    Q_ASSERT(c);
    return c->stash(QStringLiteral(STASH_KEY)).value<Error>();
}

void Error::toStash(Cutelyst::Context *c, Error::Type type, const QString &errorText, bool detach)
{
    Error e(type, errorText);
    e.toStash(c, detach);
}

#include "moc_error.cpp"
