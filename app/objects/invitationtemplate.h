/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_INVITATIONTEMPLATE_H
#define GIKWIMI_INVITATIONTEMPLATE_H

#include "../../common/global.h"
#include "event.h"
#include "guestgroup.h"
#include "guest.h"
#include "simpleuser.h"
#include "optionitem.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QJsonObject>

#include <vector>

namespace Cutelyst {
class Context;
}
class Error;
class InvitationTemplateData;

class InvitationTemplate
{
    Q_GADGET
    Q_PROPERTY(dbid_t id READ id CONSTANT)
    Q_PROPERTY(Event event READ event CONSTANT)
    Q_PROPERTY(InvitationTemplate::Type type READ type CONSTANT)
    Q_PROPERTY(GuestGroup::Salutation salutation READ salutation CONSTANT)
    Q_PROPERTY(Guest::Notification notification READ notification CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString subject READ subject CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(QDateTime updated READ updated CONSTANT)
    Q_PROPERTY(QDateTime lockedAt READ lockedAt CONSTANT)
    Q_PROPERTY(SimpleUser lockedBy READ lockedBy CONSTANT)
public:
    enum Type : int {
        Invalid = 0,
        Text    = 1,
        HTML    = 2
    };
    Q_ENUM(Type)

    InvitationTemplate();

    InvitationTemplate(dbid_t id, const Event &event, InvitationTemplate::Type type, GuestGroup::Salutation salutation, Guest::Notification notification, const QString &name, const QString &subject, const QString &text, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    InvitationTemplate(dbid_t id, dbid_t eventId, InvitationTemplate::Type type, GuestGroup::Salutation salutation, Guest::Notification notification, const QString &name, const QString &subject, const QString &text, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy);

    InvitationTemplate(const InvitationTemplate &other);

    InvitationTemplate(InvitationTemplate &&other) noexcept;

    InvitationTemplate &operator=(const InvitationTemplate &other);

    InvitationTemplate &operator=(InvitationTemplate &&other) noexcept;

    ~InvitationTemplate();

    dbid_t id() const;

    Event event() const;

    InvitationTemplate::Type type() const;

    GuestGroup::Salutation salutation() const;

    Guest::Notification notification() const;

    QString name() const;

    QString subject() const;

    QString text() const;

    QDateTime created() const;

    QDateTime updated() const;

    QDateTime lockedAt() const;

    SimpleUser lockedBy() const;

    bool isValid() const;

    bool isNull() const;

    QJsonObject toJson() const;

    static InvitationTemplate::Type typeStringToEnum(const QString &str);

    static QString typeEnumToString(InvitationTemplate::Type type);

    static QStringList supportedTypes();

    static std::vector<OptionItem> typeOptionList(Cutelyst::Context *c, InvitationTemplate::Type selected = InvitationTemplate::Invalid);

    static QStringList typeValues();

    static std::vector<InvitationTemplate> list(Cutelyst::Context *c, Error *e, dbid_t eventId);

    static std::vector<InvitationTemplate> list(Cutelyst::Context *c, Error *e, const Event &event);

    static InvitationTemplate create(Cutelyst::Context *c, Error *e, const Event &event, const QVariantHash &p);

    static InvitationTemplate get(Cutelyst::Context *c, Error *e, dbid_t id);

    bool update(Cutelyst::Context *c, Error *e, const QVariantHash &p);

private:
    QSharedDataPointer<InvitationTemplateData> d;

    friend QDataStream &operator<<(QDataStream &stream, const InvitationTemplate &templ);
    friend QDataStream &operator>>(QDataStream &stream, InvitationTemplate &templ);
};

Q_DECLARE_METATYPE(InvitationTemplate)
Q_DECLARE_TYPEINFO(InvitationTemplate, Q_MOVABLE_TYPE);

QDataStream &operator<<(QDataStream &stream, const InvitationTemplate &templ);

QDataStream &operator>>(QDataStream &stream, InvitationTemplate &templ);

#endif // GIKWIMI_INVITATIONTEMPLATE_H
