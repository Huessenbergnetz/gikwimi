/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_ADDRESSBOOK_H
#define GIKWIMI_ADDRESSBOOK_H

#include "user.h"
#include "simpleuser.h"
#include "global.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QVariant>
#include <QDateTime>
#include <vector>

namespace Cutelyst {
class Context;
}

class Error;
class AddressBookData;
class AddressBook
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(AddressBook::Type type READ type CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QVariantHash settings READ settings CONSTANT)
    Q_PROPERTY(User user READ user CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
public:
    enum Type : quint8 {
        Invalid = 0,
        Local = 1
    };
    Q_ENUM(Type)

    AddressBook();

    AddressBook(dbid_t id, AddressBook::Type type, const QString &name, const QVariantHash &settings, const QDateTime &created, const QDateTime &updated, const User &user, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    AddressBook(const AddressBook &other);

    AddressBook(AddressBook &&other) noexcept;

    AddressBook &operator=(const AddressBook &other);

    AddressBook &operator=(AddressBook &&other) noexcept;

    ~AddressBook();

    void swap(AddressBook &other) noexcept;

    dbid_t id() const;

    AddressBook::Type type() const;

    QString name() const;

    QVariantHash settings() const;

    User user() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    bool isValid() const;

    bool toStash(Cutelyst::Context *c) const;

    static bool toStash(Cutelyst::Context *c, dbid_t id);

    static AddressBook fromStash(Cutelyst::Context *c);

    static AddressBook create(Cutelyst::Context *c, Error *e, dbid_t userId, AddressBook::Type type, const QString &name, const QVariantHash &settings);

    static AddressBook create(Cutelyst::Context *c, Error *e, const User &user, AddressBook::Type type, const QString &name, const QVariantHash &settings);

    static std::vector<AddressBook> list(Cutelyst::Context *c, Error *e, dbid_t userId);

    static std::vector<AddressBook> list(Cutelyst::Context *c, Error *e, const User &user);

    static AddressBook get(Cutelyst::Context *c, Error *e, dbid_t id);

    static AddressBook::Type typeStringToEnum(const QString &str);

    static QString typeEnumToString(AddressBook::Type type);

    static QStringList supportedTypes();

protected:
    QSharedDataPointer<AddressBookData> d;

    friend QDataStream &operator<<(QDataStream &stream, const AddressBook &ab);
    friend QDataStream &operator>>(QDataStream &stream, AddressBook &ab);
};

Q_DECLARE_METATYPE(AddressBook)
Q_DECLARE_TYPEINFO(AddressBook, Q_MOVABLE_TYPE);

QDebug operator<<(QDebug dbg, const AddressBook &addressbook);

QDataStream &operator<<(QDataStream &stream, const AddressBook &ab);

QDataStream &operator>>(QDataStream &stream, AddressBook &ab);

#endif // GIKWIMI_ADDRESSBOOK_H
