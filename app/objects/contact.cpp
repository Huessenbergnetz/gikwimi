/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "contact_p.h"
#include "logging.h"
#include "error.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <KContacts/VCardConverter>

#include <QSqlQuery>
#include <QSqlError>
#include <QDataStream>
#include <QJsonArray>
#include <QJsonValue>
#include <QMetaObject>
#include <QMetaProperty>

#define CONTACT_STASH_KEY "contact"

Contact::Contact()
{

}

Contact::Contact(dbid_t id, const AddressBook &addressbook, const KContacts::Addressee &addressee, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new ContactData)
{
    d->id = id;
    d->addressbook = addressbook;
    d->addressee = addressee;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    if (lockedAt.isValid()) {
        d->lockedAt.setTimeSpec(Qt::UTC);
    }
    d->lockedBy = lockedBy;
}

Contact::Contact(dbid_t id, dbid_t addressbookId, const KContacts::Addressee &addressee, const QDateTime &created, const QDateTime &updated, const QDateTime &lockedAt, const SimpleUser &lockedBy)
    : d(new ContactData)
{
    d->id = id;
    d->addressbookId = addressbookId;
    d->addressee = addressee;
    d->created = created;
    d->created.setTimeSpec(Qt::UTC);
    d->updated = updated;
    d->updated.setTimeSpec(Qt::UTC);
    d->lockedAt = lockedAt;
    if (lockedAt.isValid()) {
        d->lockedAt.setTimeSpec(Qt::UTC);
    }
    d->lockedBy = lockedBy;
}

Contact::Contact(const Contact &other) = default;

Contact::Contact(Contact &&other) noexcept = default;

Contact &Contact::operator=(const Contact &other) = default;

Contact &Contact::operator=(Contact &&other) noexcept = default;

Contact::~Contact() = default;

void Contact::swap(Contact &other) noexcept
{
    std::swap(d, other.d);
}

dbid_t Contact::id() const
{
    return d ? d->id : 0;
}

AddressBook Contact::addressbook() const
{
    if (d) {
        if (!d->addressbook.isValid() && d->addressbookId > 0) {
            return AddressBook::get(nullptr, nullptr, d->addressbookId);
        } else {
            return d->addressbook;
        }
    } else {
        return AddressBook();
    }
}

KContacts::Addressee Contact::addressee() const
{
    return d ? d->addressee : KContacts::Addressee();
}

QDateTime Contact::created() const
{
    return d ? d->created : QDateTime();
}

QDateTime Contact::updated() const
{
    return d ? d->updated : QDateTime();
}

QDateTime Contact::lockedAt() const
{
    return d ? d->lockedAt : QDateTime();
}

SimpleUser Contact::lockedBy() const
{
    return d ? d->lockedBy : SimpleUser();
}

bool Contact::isValid() const
{
    return d && d->id > 0;
}

bool Contact::isNull() const
{
    return d ? false : true;
}

QJsonObject Contact::toJson() const
{
    QJsonObject o;

    if (isNull() || !isValid()) {
        return o;
    }

    o.insert(QStringLiteral("id"), static_cast<qint64>(d->id));
    o.insert(QStringLiteral("addressbook"), addressbook().toJson());
    o.insert(QStringLiteral("created"), d->created.toString(Qt::ISODate));
    o.insert(QStringLiteral("updated"), d->updated.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedAt"), d->lockedAt.toString(Qt::ISODate));
    o.insert(QStringLiteral("lockedBy"), d->lockedBy.toJson());


    QJsonObject ao;

    const KContacts::Addressee addressee = d->addressee;
    const QMetaObject mo = KContacts::Addressee::staticMetaObject;

    for (int i = mo.propertyOffset(); i < mo.propertyCount(); ++i) {
        const QMetaProperty mp = mo.property(i);
        const QString propName = QString::fromLatin1(mp.name());
        const QVariant propValVar = mp.readOnGadget(&addressee);

        switch(mp.type()) {
        case QMetaType::QString:
        {
            const QString str = propValVar.toString();
            if (!str.isEmpty()) {
                ao.insert(propName, str);
            }
        }
            break;
        case QMetaType::QDateTime:
        {
            const QDateTime dt = propValVar.toDateTime();
            if (dt.isValid()) {
                ao.insert(propName, dt.toString(Qt::ISODate));
            }
        }
            break;
        case QMetaType::QDate:
        {
            const QDate date = propValVar.toDate();
            if (date.isValid()) {
                ao.insert(propName, date.toString(Qt::ISODate));
            }
        }
            break;
        case QMetaType::Bool:
            ao.insert(propName, propValVar.toBool());
            break;
        case QMetaType::QUrl:
        {
            const QUrl url = propValVar.toUrl();
            if (url.isValid()) {
                ao.insert(propName, url.toString());
            }
        }
            break;
        case QMetaType::QStringList:
        {
            const QStringList lst = propValVar.toStringList();
            if (!lst.empty()) {
                ao.insert(propName, QJsonArray::fromStringList(lst));
            }
        }
            break;
        default:
            break;
        }

        const int typeInt = static_cast<int>(mp.type());
        if (typeInt == qMetaTypeId<KContacts::Geo>()) {
            const auto geo = propValVar.value<KContacts::Geo>();
            if (geo.isValid()) {
                QJsonObject geoObject;
                geoObject.insert(QStringLiteral("latitude"), geo.latitude());
                geoObject.insert(QStringLiteral("longitude"), geo.longitude());
                ao.insert(propName, geoObject);
            }
        } else if (typeInt == qMetaTypeId<KContacts::ResourceLocatorUrl>()) {
            const auto url = propValVar.value<KContacts::ResourceLocatorUrl>();
            if (url.isValid()) {
                QJsonObject urlObject;
                urlObject.insert(QStringLiteral("url"), url.url().toString());
                QJsonArray typeList;
                {
                    const QMetaObject umo = KContacts::ResourceLocatorUrl::staticMetaObject;
                    const QMetaEnum ume = umo.enumerator(umo.indexOfEnumerator("TypeFlag"));
                    for (int j = 0; j < ume.keyCount(); ++j) {
                        if (url.type().testFlag(static_cast<KContacts::ResourceLocatorUrl::TypeFlag>(ume.value(j)))) {
                            typeList.append(QString::fromLatin1(ume.key(j)));
                        }
                    }
                }
                urlObject.insert(QStringLiteral("type"), typeList);
                ao.insert(propName, urlObject);
            }
        } else if (typeInt == static_cast<int>(QMetaType::QVariantList)) {

            if (propName == QLatin1String("addresses")) {
                const KContacts::Address::List alist = addressee.addresses();

                if (!alist.empty()) {
                    QJsonArray addresses;

                    const QMetaObject amo = KContacts::Address::staticMetaObject;
                    for (const KContacts::Address &a : alist) {
                        if (Q_LIKELY(!a.isEmpty())) {
                            QJsonObject addrObject;
                            for (int j = amo.propertyOffset(); j < amo.propertyCount(); ++j) {
                                const QMetaProperty amp = amo.property(j);
                                const QString aPropName = QString::fromLatin1(amp.name());
                                const QVariant aPropVal = amp.readOnGadget(&a);

                                if (amp.type() == QMetaType::QString) {
                                    addrObject.insert(aPropName, aPropVal.toString());
                                } else if (amp.type() == QMetaType::Bool) {
                                    addrObject.insert(aPropName, aPropVal.toBool());
                                } else if (static_cast<int>(amp.type()) == qMetaTypeId<KContacts::Geo>()) {
                                    const auto geo = aPropVal.value<KContacts::Geo>();
                                    if (geo.isValid()) {
                                        QJsonObject geoObject;
                                        geoObject.insert(QStringLiteral("latitude"), geo.latitude());
                                        geoObject.insert(QStringLiteral("longitude"), geo.longitude());
                                        addrObject.insert(aPropName, geoObject);
                                    }
                                }
                            }
                            addresses.append(addrObject);
                        }
                    }
                    ao.insert(propName, addresses);
                }

            } else if (propName == QLatin1String("emails")) {
                const KContacts::Email::List elist = addressee.emailList();

                if (!elist.empty()) {
                    QJsonArray emails;

                    for (const KContacts::Email &e : elist) {
                        if (Q_LIKELY(e.isValid())) {
                            QJsonObject emailObject;
                            emailObject.insert(QStringLiteral("email"), e.mail());

                            QJsonArray typeList;
                            {
                                const QMetaObject emo = KContacts::Email::staticMetaObject;
                                const QMetaEnum eme = emo.enumerator(emo.indexOfEnumerator("TypeFlag"));
                                for (int j = 0; j < eme.keyCount(); ++j) {
                                    if (e.type().testFlag(static_cast<KContacts::Email::TypeFlag>(eme.value(j)))) {
                                        typeList.append(QString::fromLatin1(eme.key(j)));
                                    }
                                }
                            }
                            emailObject.insert(QStringLiteral("type"), typeList);
                            emails.append(emailObject);
                        }
                    }

                    ao.insert(propName, emails);
                }
            } else if (propName == QLatin1String("impps")) {
                const KContacts::Impp::List ilist = addressee.imppList();

                if (!ilist.empty()) {
                    QJsonArray impps;

                    for (const KContacts::Impp &im : ilist) {
                        QJsonObject io;
                        io.insert(QStringLiteral("address"), im.address().toString());
                        io.insert(QStringLiteral("serviceIcon"), im.serviceIcon());
                        io.insert(QStringLiteral("serviceLabel"), im.serviceLabel());
                        io.insert(QStringLiteral("serviceType"), im.serviceType());
                        impps.append(io);
                    }

                    ao.insert(propName, impps);
                }
            } else if (propName == QLatin1String("phoneNumbers")) {
                const KContacts::PhoneNumber::List plist = addressee.phoneNumbers();

                if (!plist.empty()) {
                    QJsonArray phoneNumbers;

                    for (const KContacts::PhoneNumber &p : plist) {
                        QJsonObject po;
                        po.insert(QStringLiteral("id"), p.id());
                        po.insert(QStringLiteral("isPreferred"), p.isPreferred());
                        po.insert(QStringLiteral("normalizedNumber"), p.normalizedNumber());
                        po.insert(QStringLiteral("number"), p.number());
                        po.insert(QStringLiteral("supportsSms"), p.supportsSms());

                        QJsonArray typeList;
                        {
                            const QMetaObject pmo = KContacts::PhoneNumber::staticMetaObject;
                            const QMetaEnum pme = pmo.enumerator(pmo.indexOfEnumerator("TypeFlag"));
                            for (int j = 0; j < pme.keyCount(); ++j) {
                                if (p.type().testFlag(static_cast<KContacts::PhoneNumber::TypeFlag>(pme.value(j)))) {
                                    typeList.append(QString::fromLatin1(pme.key(j)));
                                }
                            }
                        }
                        po.insert(QStringLiteral("type"), typeList);

                        phoneNumbers.append(po);
                    }

                    ao.insert(propName, phoneNumbers);
                }
            } else if (propName == QLatin1String("urls")) {
                const KContacts::ResourceLocatorUrl::List ulist = addressee.extraUrlList();

                if (!ulist.empty()) {
                    QJsonArray urls;

                    for (const KContacts::ResourceLocatorUrl &u : ulist) {
                        QJsonObject uo;
                        uo.insert(QStringLiteral("url"), u.url().toString());

                        QJsonArray typeList;
                        {
                            const QMetaObject umo = KContacts::ResourceLocatorUrl::staticMetaObject;
                            const QMetaEnum ume = umo.enumerator(umo.indexOfEnumerator("TypeFlag"));
                            for (int j = 0; j < ume.keyCount(); ++j) {
                                if (u.type().testFlag(static_cast<KContacts::ResourceLocatorUrl::TypeFlag>(ume.value(j)))) {
                                    typeList.append(QString::fromLatin1(ume.key(j)));
                                }
                            }
                        }
                        uo.insert(QStringLiteral("type"), typeList);

                        urls.append(uo);
                    }

                    ao.insert(propName, urls);
                }
            }
        }
    }

    o.insert(QStringLiteral("addressee"), ao);

    return o;
}

std::vector<Contact> Contact::list(Cutelyst::Context *c, Error *e, const AddressBook &addressbook)
{
    std::vector<Contact> contacts;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT c.id, c.vcard, c.created_at, c.updated_at, c.locked_at, u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM contacts c LEFT JOIN users u ON u.id = c.locked_by WHERE addressbook_id = :addressbook_id ORDER BY c.family_name, c.given_name"));
    q.bindValue(QStringLiteral(":addressbook_id"), addressbook.id());

    if (Q_UNLIKELY(!q.exec())) {
        if (c && e) *e = Error(q.lastError(), c->translate("Contact", "Failed to query contacts from database."));
        qCCritical(GIK_CORE) << "Failed to query contacts in" << addressbook << "from database:" << q.lastError().text();
        return contacts;
    }

    if (q.size() > -1) {
        contacts.reserve(q.size());
    }

    KContacts::VCardConverter converter;
    while (q.next()) {
        const dbid_t id                         = q.value(0).toUInt();
        const QByteArray vcard                  = q.value(1).toString().toUtf8();
        const KContacts::Addressee addressee    = converter.parseVCard(vcard);
        const QDateTime created                 = q.value(2).toDateTime();
        const QDateTime updated                 = q.value(3).toDateTime();
        const qlonglong lockedAt                = q.value(4).toLongLong();
        const SimpleUser lockedBy               = lockedAt > 0 ? SimpleUser(q.value(5).toUInt(), q.value(6).toString()) : SimpleUser();
        if (Q_UNLIKELY(addressee.isEmpty())) {
            qCWarning(GIK_CORE) << "Failed to parse vCard data for contact ID" << id;
        } else {
            contacts.emplace_back(id, addressbook, addressee, created, updated, QDateTime::fromMSecsSinceEpoch(lockedAt), lockedBy);
        }
    }

    return contacts;
}

Contact Contact::get(Cutelyst::Context *c, Error *e, dbid_t id)
{
    Contact contact;

    QSqlQuery q = CPreparedSqlQueryThreadFO(QStringLiteral("SELECT c.addressbook_id, c.vcard, c.created_at, c.updated_at, c.locked_at, u.id AS locked_by_id, u.username AS locked_by_username "
                                                           "FROM contacts c LEFT JOIN users u ON u.id = c.locked_by WHERE c.id = :id"));
    q.bindValue(QStringLiteral(":id"), id);

    if (Q_LIKELY(q.exec())) {

        if (q.next()) {
            const QByteArray vcard = q.value(1).toString().toUtf8();
            KContacts::VCardConverter converter;
            const KContacts::Addressee addressee = converter.parseVCard(vcard);
            if (addressee.isEmpty()) {
                qCWarning(GIK_CORE) << "Failed to parse vCard data for contact ID" <<id;
            }

            const qlonglong  lat      = q.value(4).toLongLong();
            const QDateTime  lockedAt = lat > 0 ? QDateTime::fromMSecsSinceEpoch(lat) : QDateTime();
            const SimpleUser lockedBy = lat > 0 ? SimpleUser(q.value(5).toUInt(), q.value(6).toString()) : SimpleUser();

            contact = Contact(id,
                              q.value(0).toUInt(),
                              addressee,
                              q.value(2).toDateTime(),
                              q.value(3).toDateTime(),
                              lockedAt,
                              lockedBy);
        } else {
            if (c && e) *e = Error(Error::NotFound, c->translate("Contact", "Can not find contact ID %1 in the database.").arg(id));
            qCWarning(GIK_CORE) << "Can not find contact ID" <<id << "in the database.";
        }

    } else {
        if (c && e) *e = Error(q.lastError(), c->translate("Contact", "Failed to get contact ID %1 from the database.").arg(id));
        qCCritical(GIK_CORE) << "Failed to query contact ID" << id << "from the database:" << q.lastError().text();
    }

    return contact;
}

QDataStream &operator<<(QDataStream &stream, const Contact &contact)
{
    stream << contact.d->addressee
           << contact.d->addressbook
           << contact.d->user
           << contact.d->created
           << contact.d->updated
           << contact.d->lockedAt
           << contact.d->lockedBy
           << contact.d->id
           << contact.d->addressbookId;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Contact &contact)
{
    if (contact.d == nullptr) {
        contact.d = new ContactData;
    }

    stream >> contact.d->addressee;
    stream >> contact.d->addressbook;
    stream >> contact.d->user;
    stream >> contact.d->created;
    stream >> contact.d->updated;
    stream >> contact.d->lockedAt;
    stream >> contact.d->lockedBy;
    stream >> contact.d->id;
    stream >> contact.d->addressbookId;
    return stream;
}

#include "moc_contact.cpp"
