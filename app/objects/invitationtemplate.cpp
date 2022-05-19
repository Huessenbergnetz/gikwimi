/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "invitationtemplate.h"
#include "error.h"
#include "../logging.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QMetaObject>
#include <QMetaEnum>
#include <QSqlQuery>
#include <QSqlError>

class InvitationTemplateData : public QSharedData
{
public:
    Event event;
    QString name;
    QString subject;
    QString text;
    QDateTime created;
    QDateTime updated;
    QDateTime lockedAt;
    SimpleUser lockedBy;
    dbid_t id = 0;
    dbid_t eventId = 0;
    GuestGroup::Salutation salutation = GuestGroup::SalutationInvalid;
    InvitationTemplate::Type type = InvitationTemplate::Invalid;
    Guest::Notification notification = Guest::NotNotified;
};

InvitationTemplate::InvitationTemplate()
{

}

InvitationTemplate::InvitationTemplate(dbid_t id, const Event &event, InvitationTemplate::Type type, GuestGroup::Salutation salutation, Guest::Notification notification, const QString &name, const QString &subject, const QString &text, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new InvitationTemplateData)
{
    d->id = id;
    d->event = event;
    d->type = type;
    d->salutation = salutation;
    d->notification = notification;
    d->name = name;
    d->subject = subject;
    d->text = text;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    d->lockedBy = lockedBy;
}

InvitationTemplate::InvitationTemplate(dbid_t id, dbid_t eventId, InvitationTemplate::Type type, GuestGroup::Salutation salutation, Guest::Notification notification, const QString &name, const QString &subject, const QString &text, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new InvitationTemplateData)
{
    d->id = id;
    d->eventId = eventId;
    d->type = type;
    d->salutation = salutation;
    d->notification = notification;
    d->name = name;
    d->subject = subject;
    d->text = text;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    d->lockedBy = lockedBy;
}

InvitationTemplate::InvitationTemplate(const InvitationTemplate &other) = default;

InvitationTemplate::InvitationTemplate(InvitationTemplate &&other) noexcept = default;

InvitationTemplate &InvitationTemplate::operator=(const InvitationTemplate &other) = default;

InvitationTemplate &InvitationTemplate::operator=(InvitationTemplate &&other) noexcept = default;

InvitationTemplate::~InvitationTemplate() = default;

dbid_t InvitationTemplate::id() const
{
    return d ? d->id : 0;
}

Event InvitationTemplate::event() const
{
    if (d) {
        if (!d->event.isValid() && d->eventId > 0) {
            return Event::get(nullptr, nullptr, d->eventId);
        } else {
            return d->event;
        }
    } else {
        return Event();
    }
}

InvitationTemplate::Type InvitationTemplate::type() const
{
    return d ? d->type : InvitationTemplate::Invalid;
}

GuestGroup::Salutation InvitationTemplate::salutation() const
{
    return d ? d->salutation : GuestGroup::SalutationInvalid;
}

Guest::Notification InvitationTemplate::notification() const
{
    return d ? d->notification : Guest::NotNotified;
}

QString InvitationTemplate::name() const
{
    return d ? d->name : QString();
}

QString InvitationTemplate::subject() const
{
    return d ? d->subject: QString();
}

QString InvitationTemplate::text() const
{
    return d ? d->text : QString();
}

QDateTime InvitationTemplate::created() const
{
    return d ? d->created : QDateTime();
}

QDateTime InvitationTemplate::updated() const
{
    return d ? d->updated : QDateTime();
}

QDateTime InvitationTemplate::lockedAt() const
{
    return d ? d->lockedAt : QDateTime();
}

SimpleUser InvitationTemplate::lockedBy() const
{
    return d ? d->lockedBy : SimpleUser();
}

bool InvitationTemplate::isValid() const
{
    return d && d->id > 0;
}

bool InvitationTemplate::isNull() const
{
    return d ? false : true;
}

QJsonObject InvitationTemplate::toJson() const
{
    QJsonObject o;

    if (isNull() || !isValid()) {
        return o;
    }

    o.insert(QStringLiteral("id"),              static_cast<qint64>(d->id));
    o.insert(QStringLiteral("event"),           d->event.toJson());
    o.insert(QStringLiteral("type"),            InvitationTemplate::typeEnumToString(d->type));
    o.insert(QStringLiteral("salutation"),      GuestGroup::salutationEnumToString(d->salutation));
    o.insert(QStringLiteral("notification"),    Guest::notificationEnumToString(d->notification));
    o.insert(QStringLiteral("name"),            d->name);
    o.insert(QStringLiteral("subject"),         d->subject);
    o.insert(QStringLiteral("text"),            d->text);
    o.insert(QStringLiteral("createdAt"),       d->created.toString(Qt::ISODate));
    o.insert(QStringLiteral("updatedAt"),       d->updated.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedAt"),        d->lockedAt.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedBy"),        d->lockedBy.toJson());

    return o;
}

InvitationTemplate::Type InvitationTemplate::typeStringToEnum(const QString &str)
{
    bool ok = false;
    const int type = str.toInt(&ok);
    if (ok) {
        return static_cast<InvitationTemplate::Type>(type);
    } else {
        if (str.compare(QLatin1String("text"), Qt::CaseInsensitive) == 0) {
            return InvitationTemplate::Text;
        } else if (str.compare(QLatin1String("html"), Qt::CaseInsensitive) == 0) {
            return InvitationTemplate::HTML;
        } else {
            qCWarning(GIK_CORE) << "Can not convert string to InvitationTemplate::Type:" << str << "is neither a valid integer value nor a valid enum key.";
            return InvitationTemplate::Invalid;
        }
    }
}

QString InvitationTemplate::typeEnumToString(Type type)
{
    QString str;

    if (type != InvitationTemplate::Invalid) {
        const QMetaObject mo = InvitationTemplate::staticMetaObject;
        const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));
        str = QString::fromLatin1(me.valueToKey(static_cast<int>(type))).toLower();
    }

    return str;
}

QStringList InvitationTemplate::supportedTypes()
{
    QStringList lst;

    const QMetaObject mo = InvitationTemplate::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));

    lst.reserve(me.keyCount() - 1);

    for (int i = 1; i < me.keyCount(); ++i) {
        lst.push_back(QString::fromLatin1(me.key(i)));
    }

    return lst;
}

std::vector<OptionItem> InvitationTemplate::typeOptionList(Cutelyst::Context *c, Type selected)
{
    std::vector<OptionItem> options;

    options.emplace_back(c->translate("InvitationTemplate", "Select template type"), static_cast<int>(InvitationTemplate::Invalid), selected == InvitationTemplate::Invalid);
    options.emplace_back(c->translate("InvitationTemplate", "Plain text"), static_cast<int>(InvitationTemplate::Text), selected == InvitationTemplate::Text);
    options.emplace_back(c->translate("InvitationTemplate", "HTML"), static_cast<int>(InvitationTemplate::HTML), selected == InvitationTemplate::HTML);

    return options;
}

QStringList InvitationTemplate::typeValues()
{
    QStringList lst;

    const QMetaObject mo = InvitationTemplate::staticMetaObject;
    const QMetaEnum   me = mo.enumerator(mo.indexOfEnumerator("Type"));

    lst.reserve(me.keyCount() - 1);

    const int startIndex = 1;
    for (int i = startIndex; i < me.keyCount(); ++i) {
        lst.push_back(QString::number(me.value(i)));
    }

    return lst;
}

std::vector<InvitationTemplate> InvitationTemplate::list(Cutelyst::Context *c, Error *e, dbid_t eventId)
{
    std::vector<InvitationTemplate> templates;

    Event event = Event::fromStash(c);
    if (event.id() != eventId) {
        event = Event::get(c, e, eventId);
        if (e && e->type() != Error::NoError) {
            return templates;
        }
    }
    templates = InvitationTemplate::list(c, e, event);

    return templates;
}

std::vector<InvitationTemplate> InvitationTemplate::list(Cutelyst::Context *c, Error *e, const Event &event)
{
    std::vector<InvitationTemplate> templates;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT t.id, t.type, t.salutation, t.notification, t.name, t.subject, t.text, t.created_at, t.updated_at, t.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM templates t LEFT JOIN users u ON u.id = t.locked_by WHERE t.event_id = :event_id"));
    q.bindValue(QStringLiteral(":event_id"), event.id());

    if (Q_LIKELY(q.exec())) {

        if (q.size() > 0) {
            templates.reserve(q.size());
        }

        while (q.next()) {

            const qlonglong  lat        = q.value(9).toLongLong();
            const QDateTime  lockedAt   = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy   = lat > 0 ? SimpleUser(q.value(10).toUInt(), q.value(11).toString()) : SimpleUser();

            templates.emplace_back(q.value(0).toUInt(),
                                   event,
                                   static_cast<InvitationTemplate::Type>(q.value(1).toInt()),
                                   static_cast<GuestGroup::Salutation>(q.value(2).toInt()),
                                   static_cast<Guest::Notification>(q.value(3).toInt()),
                                   q.value(4).toString(),
                                   q.value(5).toString(),
                                   q.value(6).toString(),
                                   q.value(7).toDateTime(),
                                   q.value(8).toDateTime(),
                                   lockedAt,
                                   lockedBy);
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("InvitationTemplate", "Failed to query templates from the database."));
        qCCritical(GIK_CORE) << "Failed to to query templates for event ID" << event.id() << "from the database:" << q.lastError().text();
    }

    return templates;
}

InvitationTemplate InvitationTemplate::create(Cutelyst::Context *c, Error *e, const Event &event, const QVariantHash &p)
{
    InvitationTemplate templ;

    Q_ASSERT(event.isValid());

    const InvitationTemplate::Type  type            = static_cast<InvitationTemplate::Type>(p.value(QStringLiteral("type")).toInt());
    const GuestGroup::Salutation    salutation      = static_cast<GuestGroup::Salutation>(p.value(QStringLiteral("salutation")).toInt());
    const Guest::Notification       notification    = static_cast<Guest::Notification>(p.value(QStringLiteral("notification")).toInt());
    const QString                   name            = p.value(QStringLiteral("name")).toString();
    const QString                   subject         = p.value(QStringLiteral("subject")).toString();
    const QString                   text            = p.value(QStringLiteral("text")).toString();
    const QDateTime                 now             = QDateTime::currentDateTimeUtc();

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("INSERT INTO "
                                                         "templates (event_id, type, salutation, notification, name, subject, text, created_at, updated_at) "
                                                         "VALUES (:even_id, :type, :salutation, :notification, :name, :subject, :text, :created_at, :updated_at)"));

    q.bindValue(QStringLiteral(":even_id"),         event.id());
    q.bindValue(QStringLiteral(":type"),            type);
    q.bindValue(QStringLiteral(":salutation"),      salutation);
    q.bindValue(QStringLiteral(":notification"),    notification);
    q.bindValue(QStringLiteral(":name"),            name);
    q.bindValue(QStringLiteral(":subject"),         subject);
    q.bindValue(QStringLiteral(":text"),            text);
    q.bindValue(QStringLiteral(":created_at"),      now);
    q.bindValue(QStringLiteral(":updated_at"),      now);

    if (Q_LIKELY(q.exec())) {
        const dbid_t id = q.lastInsertId().toUInt();
        templ = InvitationTemplate(id, event, type, salutation, notification, name, subject, text, now, now, QDateTime(), SimpleUser());
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("InvitationTemplate", "Failed to create new template “%1” for event “%2” (ID: %3) in the database.").arg(name, event.title(), QString::number(event.id())));
        qCCritical(GIK_CORE) << "Failed to execute database query to create new template" << name << "for event ID" << event.id() << "in the database:" << q.lastError().text();
    }

    return templ;
}

InvitationTemplate InvitationTemplate::get(Cutelyst::Context *c, Error *e, dbid_t id)
{
    InvitationTemplate templ;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT t.event_id, t.type, t.salutation, t.notification, t.name, t.subject, t.text, t.created_at, t.updated_at, t.locked_at, "
                                                           "u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM templates t LEFT JOIN users u ON u.id = t.locked_by WHERE t.id = :id"));
    q.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q.exec())) {
        if (Q_LIKELY(q.next())) {

            const qlonglong  lat      = q.value(9).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(10).toUInt(), q.value(11).toString()) : SimpleUser();

            templ = InvitationTemplate(id,
                                       q.value(0).toUInt(),
                                       static_cast<InvitationTemplate::Type>(q.value(1).toInt()),
                                       static_cast<GuestGroup::Salutation>(q.value(2).toInt()),
                                       static_cast<Guest::Notification>(q.value(3).toInt()),
                                       q.value(4).toString(),
                                       q.value(5).toString(),
                                       q.value(6).toString(),
                                       q.value(7).toDateTime(),
                                       q.value(8).toDateTime(),
                                       lockedAt,
                                       lockedBy);

        } else {
            if (c && e) *e = Error(Error::NotFound, c->translate("InvitationTemplate", "Can not find template ID %1 in the database.").arg(id));
            qCWarning(GIK_CORE) << "Can not find template ID" << id << "in the database";
        }
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("InvitationTemplate", "Failed to get template ID %1 from the database.").arg(id));
        qCCritical(GIK_CORE) << "Failed to get template ID" << id << "from the database:" << q.lastError().text();
    }

    return templ;
}

bool InvitationTemplate::update(Cutelyst::Context *c, Error *e, const QVariantHash &p)
{
    const InvitationTemplate::Type  type            = static_cast<InvitationTemplate::Type>(p.value(QStringLiteral("type")).toInt());
    const GuestGroup::Salutation    salutation      = static_cast<GuestGroup::Salutation>(p.value(QStringLiteral("salutation")).toInt());
    const Guest::Notification       notification    = static_cast<Guest::Notification>(p.value(QStringLiteral("notification")).toInt());
    const QString                   name            = p.value(QStringLiteral("name")).toString();
    const QString                   subject         = p.value(QStringLiteral("subject")).toString();
    const QString                   text            = p.value(QStringLiteral("text")).toString();
    const QDateTime                 now             = QDateTime::currentDateTimeUtc();

    QSqlQuery q = CPreparedSqlQueryThread(QStringLiteral("UPDATE templates SET "
                                                         "type = :type, "
                                                         "salutation = :salutation, "
                                                         "notification = :notification, "
                                                         "name = :name, "
                                                         "subject = :subject, "
                                                         "text = :text,"
                                                         "updated_at = :updated_at "
                                                         "WHERE id = :id"));
    q.bindValue(QStringLiteral(":type"),         static_cast<int>(type));
    q.bindValue(QStringLiteral(":salutation"),   static_cast<int>(salutation));
    q.bindValue(QStringLiteral(":notification"), static_cast<int>(notification));
    q.bindValue(QStringLiteral(":name"),         name);
    q.bindValue(QStringLiteral(":subject"),      subject);
    q.bindValue(QStringLiteral(":text"),         text);
    q.bindValue(QStringLiteral(":updated_at"),   now);
    q.bindValue(QStringLiteral(":id"),           id());

    if (Q_LIKELY(q.exec())) {
        d->type = type;
        d->salutation = salutation;
        d->notification = notification;
        d->name = name;
        d->subject = subject;
        d->text = text;
        d->updated = now;
        return true;
    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("InvitationTemplate", "Failed to update invitation template in the database."));
        qCCritical(GIK_CORE) << "Failed to update invitation template ID" << id() << "in the database:" << q.lastError().text();
        return false;
    }
}

QDataStream &operator<<(QDataStream &stream, const InvitationTemplate &templ)
{
    stream << templ.d->id
           << templ.d->eventId
           << templ.d->event
           << static_cast<qint32>(templ.d->type)
           << static_cast<qint32>(templ.d->salutation)
           << static_cast<qint32>(templ.d->notification)
           << templ.d->name
           << templ.d->subject
           << templ.d->text
           << templ.d->created
           << templ.d->updated
           << templ.d->lockedAt
           << templ.d->lockedBy;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, InvitationTemplate &templ)
{
    stream >> templ.d->id;
    stream >> templ.d->eventId;
    stream >> templ.d->event;
    qint32 typeInt;
    stream >> typeInt;
    templ.d->type = static_cast<InvitationTemplate::Type>(typeInt);
    qint32 salInt;
    stream >> salInt;
    templ.d->salutation = static_cast<GuestGroup::Salutation>(salInt);
    qint32 notInt;
    stream >> notInt;
    templ.d->notification = static_cast<Guest::Notification>(notInt);
    stream >> templ.d->name;
    stream >> templ.d->subject;
    stream >> templ.d->text;
    stream >> templ.d->created;
    stream >> templ.d->updated;
    stream >> templ.d->lockedAt;
    stream >> templ.d->lockedBy;
    return stream;
}

#include "moc_invitationtemplate.cpp"
