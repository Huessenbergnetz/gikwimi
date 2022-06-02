/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenterevents.h"
#include "../objects/user.h"
#include "../objects/event.h"
#include "../objects/error.h"
#include "../objects/guest.h"
#include "../objects/invitationtemplate.h"
#include "../objects/optionitem.h"
#include "../utils.h"

#include <Cutelyst/Plugins/Utils/Validator> // includes the main validator
#include <Cutelyst/Plugins/Utils/ValidatorResult> // includes the validator result
#include <Cutelyst/Plugins/Utils/validatorrequired.h>
#include <Cutelyst/Plugins/Utils/validatoralphadash.h>
#include <Cutelyst/Plugins/Utils/validatorbetween.h>
#include <Cutelyst/Plugins/Utils/validatorin.h>
#include <Cutelyst/Plugins/Utils/validatorrequiredif.h>
#include <Cutelyst/Plugins/StatusMessage>

#include <QJsonObject>
#include <QJsonValue>

#include <limits>

ControlCenterEvents::ControlCenterEvents(QObject *parent)
    : Controller{parent}
{

}

ControlCenterEvents::~ControlCenterEvents() = default;

void ControlCenterEvents::index(Context *c)
{
    const User currentUser = User::fromStash(c);

    Error e;
    const std::vector<Event> events = Event::list(c, &e, currentUser);

    if (e.type() != Error::NoError) {
        e.toStash(c, true);
        return;
    }

    QMap<QString,QString> eventsTableHeaders;
    //: table header
    eventsTableHeaders.insert(QStringLiteral("id"), c->translate("ControlCenterEvents", "ID"));
    //: table header
    eventsTableHeaders.insert(QStringLiteral("title"), c->translate("ControlCenterEvents", "title"));
    //: table header
    eventsTableHeaders.insert(QStringLiteral("schedule"), c->translate("ControlCenterEvents", "schedule"));
    //: table header
    eventsTableHeaders.insert(QStringLiteral("guests"), c->translate("ControlCenterEvents", "guests"));

    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterEvents", "Events")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/index.tmpl")},
                 {QStringLiteral("events"), QVariant::fromValue<std::vector<Event>>(events)},
                 {QStringLiteral("events_table_headers"), QVariant::fromValue<QMap<QString,QString>>(eventsTableHeaders)}
             });
}

void ControlCenterEvents::base(Context *c, const QString &id)
{
    bool ok = false;
    const dbid_t eventId = Utils::strToDbid(id, &ok, c->translate("ControlCenterEvents", "Invalid event database ID."), c);
    if (Q_LIKELY(ok)) {
        Event::toStash(c, eventId);
    }
}

void ControlCenterEvents::edit(Context *c)
{
    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Edit")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/edit.tmpl")}
             });
}

void ControlCenterEvents::guests(Context *c)
{
    const auto event = Event::fromStash(c);

    Error e;
    const auto groups = GuestGroup::list(c, &e, event);
    if (e.type() != Error::NoError) {
        e.toStash(c, true);
        return;
    }

    QMap<QString,QString> guestsTableHeaders;
    //: table header
    guestsTableHeaders.insert(QStringLiteral("name"), c->translate("ControlCenterEvents", "name"));
    //: table header
    guestsTableHeaders.insert(QStringLiteral("guests"), c->translate("ControlCenterEvents", "guests"));
    //: table header
    guestsTableHeaders.insert(QStringLiteral("address"), c->translate("ControlCenterEvents", "address"));
    //: table header
    guestsTableHeaders.insert(QStringLiteral("notified"), c->translate("ControlCenterEvents", "notified"));
    //: table data description, number of adult guests
    guestsTableHeaders.insert(QStringLiteral("adults"), c->translate("ControlCenterEvents", "adults"));
    //: table data description, number of child guests
    guestsTableHeaders.insert(QStringLiteral("children"), c->translate("ControlCenterEvents", "children"));

    const std::vector<OptionItem> salutationOptions = GuestGroup::salutationOptionList(c);
    const std::vector<OptionItem> guestTypeOptions = Guest::typeOptionList(c);
    const std::vector<OptionItem> guestStatusOptions = Guest::statusOptionList(c);

    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Guests")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/index.tmpl")},
                 {QStringLiteral("groups"), QVariant::fromValue<std::vector<GuestGroup>>(groups)},
                 {QStringLiteral("guests_table_headers"), QVariant::fromValue<QMap<QString,QString>>(guestsTableHeaders)},
                 {QStringLiteral("salutation_options"), QVariant::fromValue<std::vector<OptionItem>>(salutationOptions)},
                 {QStringLiteral("guesttype_options"), QVariant::fromValue<std::vector<OptionItem>>(guestTypeOptions)},
                 {QStringLiteral("gueststatus_options"), QVariant::fromValue<std::vector<OptionItem>>(guestStatusOptions)}
             });
}

void ControlCenterEvents::addGuest(Context *c)
{
    const auto req = c->req();

    if (req->isPost()) {

        static Validator v({
                               new ValidatorRequired(QStringLiteral("group")),
                               new ValidatorBetween(QStringLiteral("group"), QMetaType::UInt, std::numeric_limits<uint>::min(), std::numeric_limits<uint>::max()),
                               new ValidatorRequired(QStringLiteral("contact")),
                               new ValidatorBetween(QStringLiteral("contact"), QMetaType::UInt, std::numeric_limits<uint>::min(), std::numeric_limits<uint>::max()),
                               new ValidatorRequired(QStringLiteral("expectedAdults")),
                               new ValidatorBetween(QStringLiteral("expectedAdults"), QMetaType::UInt, 0, 30),
                               new ValidatorRequired(QStringLiteral("expectedChildren")),
                               new ValidatorBetween(QStringLiteral("expectedChildren"), QMetaType::UInt, 0, 30),
                               new ValidatorRequired(QStringLiteral("salutation")),
                               new ValidatorIn(QStringLiteral("salutation"), GuestGroup::salutationValues(true)),
                               new ValidatorBetween(QStringLiteral("salutation"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("type")),
                               new ValidatorIn(QStringLiteral("type"), Guest::typeValues()),
                               new ValidatorBetween(QStringLiteral("type"), QMetaType::Int, -127, 127)
                           });
        const ValidatorResult vr = v.validate(c, Validator::FillStashOnError|Validator::BodyParamsOnly);
        if (vr) {
            QVariantHash values = vr.values();
            values.insert(QStringLiteral("partnerGivenName"), req->bodyParam(QStringLiteral("partnerGivenName")));
            values.insert(QStringLiteral("partnerFamilyName"), req->bodyParam(QStringLiteral("partnerFamilyName")));

            Error e;
            const Guest guest = Guest::create(c, &e, Event::fromStash(c), values);
            if (guest.isValid()) {
                if (req->xhr()) {
                    c->res()->setJsonObjectBody(guest.toJson());
                }
                c->res()->setStatus(201);
            } else {
                e.toStash(c, true);
                return;
            }
        } else {
            c->res()->setStatus(400);
            c->detach(c->getAction(QStringLiteral("error")));
        }
    }

    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Add guest")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/add.tmpl")}
             });
}

void ControlCenterEvents::addGuestToGroup(Context *c, const QString &groupId)
{
    c->stash({
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Add guest")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/addtogroup.tmpl")}
             });
}

void ControlCenterEvents::editGuest(Context *c, const QString &id)
{
    bool ok = false;
    const dbid_t guestId = Utils::strToDbid(id, &ok, c->translate("ControlCenterEvents", "Invalid guest database ID."), c);

    if (!ok) {
        return;
    }

    Error e;
    Guest guest = Guest::get(c, &e, guestId);

    if (!guest.isValid()) {
        e.toStash(c, true);
        return;
    }

    if (c->req()->isPost()) {
        static Validator v({
                               new ValidatorRequired(QStringLiteral("type")),
                               new ValidatorIn(QStringLiteral("type"), Guest::typeValues()),
                               new ValidatorBetween(QStringLiteral("type"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("salutation")),
                               new ValidatorIn(QStringLiteral("salutation"), GuestGroup::salutationValues(true)),
                               new ValidatorBetween(QStringLiteral("salutation"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("status")),
                               new ValidatorIn(QStringLiteral("status"), Guest::statusValues(true)),
                               new ValidatorBetween(QStringLiteral("status"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("expectedAdults")),
                               new ValidatorBetween(QStringLiteral("expectedAdults"), QMetaType::UInt, 0, 30),
                               new ValidatorRequired(QStringLiteral("agreedAdults")),
                               new ValidatorBetween(QStringLiteral("agreedAdults"), QMetaType::UInt, 0, 30),
                               new ValidatorRequired(QStringLiteral("expectedChildren")),
                               new ValidatorBetween(QStringLiteral("expectedChildren"), QMetaType::UInt, 0, 30),
                               new ValidatorRequired(QStringLiteral("agreedChildren")),
                               new ValidatorBetween(QStringLiteral("agreedChildren"), QMetaType::UInt, 0, 30),
                           });
        const ValidatorResult vr = v.validate(c, Validator::FillStashOnError|Validator::BodyParamsOnly);
        if (vr) {
            QVariantHash values = vr.values();
            values.insert(QStringLiteral("partnerGivenName"), c->req()->bodyParam(QStringLiteral("partnerGivenName")));
            values.insert(QStringLiteral("partnerFamilyName"), c->req()->bodyParam(QStringLiteral("partnerFamilyName")));

            if (guest.update(c, &e, values)) {
                if (c->req()->xhr()) {
                    c->res()->setJsonObjectBody(guest.toJson());
                    return;
                }
            } else {
                e.toStash(c, true);
                return;
            }
        } else {
            c->res()->setStatus(400);
            c->res()->setJsonObjectBody(QJsonObject({{QStringLiteral("fielderrors"), vr.errorsJsonObject()}}));
            return;
        }
    } else {
        if (c->req()->xhr()) {
            c->res()->setJsonObjectBody(guest.toJson());
            return;
        }
    }

    c->stash({
                 {QStringLiteral("guest"), QVariant::fromValue<Guest>(guest)},
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Edit guests")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/edit.tmpl")}
             });
}

void ControlCenterEvents::inviteGuest(Context *c, const QString &id, const QString &notificationValue)
{
    bool ok = false;
    const dbid_t guestId = Utils::strToDbid(id, &ok, c->translate("ControlCenterEvents", "Invalid guest database ID."), c);

    if (!ok) {
        return;
    }

    Error e;
    Guest guest = Guest::get(c, &e, guestId);

    if (!guest.isValid()) {
        e.toStash(c, true);
        return;
    }

    const Guest::Notification notification = static_cast<Guest::Notification>(notificationValue.toInt(&ok));

    if (!ok) {
        e = Error(Error::InputError, c->translate("ControlCenterEvents", "Invalid notification type identifier."));
        e.toStash(c, true);
        return;
    }

    if (c->req()->isPost()) {
        if (guest.markAsInvited(c, &e, notification)) {
            const Event event = Event::fromStash(c);
            c->res()->redirect(c->uriForAction(QStringLiteral("/controlcenter/events/guests"), QStringList(QString::number(event.id())), QStringList()));
        } else {
            e.toStash(c, true);
        }
        return;
    }

    const QString invitationText = guest.invitationText(c, &e, notification);

    if (e.type() != Error::NoError) {
        e.toStash(c, true);
        return;
    }

    const QString invitationSubject = (notification == Guest::Email || notification == Guest::Postal) ? guest.invitationSubject(c, &e, notification) : QString();

    c->stash({
                 {QStringLiteral("guest"), QVariant::fromValue<Guest>(guest)},
                 {QStringLiteral("notification_type"), static_cast<int>(notification)},
                 {QStringLiteral("invitation_subject"), invitationSubject},
                 {QStringLiteral("invitation_text"), invitationText},
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Invite guest")},
                 {QStringLiteral("site_title"), guest.contact().addressee().formattedName()},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guests/invite.tmpl")}
             });
}

void ControlCenterEvents::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterEvents", "Add Event")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/add.tmpl")}
             });
}

void ControlCenterEvents::guestGroups(Context *c)
{
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guestgroups/index.tmpl")}
             });
}

void ControlCenterEvents::addGuestGroup(Context *c)
{
    const auto req = c->req();

    if (req->isPost()) {
        static Validator v({
                               new ValidatorRequired(QStringLiteral("name")),
                               new ValidatorAlphaDash(QStringLiteral("slug"), true),
                               new ValidatorRequired(QStringLiteral("salutation")),
                               new ValidatorIn(QStringLiteral("salutation"), GuestGroup::salutationValues(false), Qt::CaseSensitive, ValidatorMessages(QT_TRANSLATE_NOOP("ControlCenterEvents", "Salutation"), QT_TRANSLATE_NOOP("ControlCenterEvents", "Please select a valid salutatuion form."))),
                               new ValidatorBetween(QStringLiteral("salutation"), QMetaType::Int, -127, 127)
                           });

        const ValidatorResult vr = v.validate(c, Validator::FillStashOnError|Validator::BodyParamsOnly);
        if (vr) {
            Error e;
            const GuestGroup group = GuestGroup::create(c, &e, Event::fromStash(c), vr.values());
            if (group.isValid()) {
                if (req->xhr()) {
                    c->res()->setJsonObjectBody(group.toJson());
                }
                c->res()->setStatus(201);
                return;
            } else {
                e.toStash(c, true);
                return;
            }
        } else {
            if (req->xhr()) {
                c->res()->setStatus(400);
                c->res()->setJsonObjectBody(QJsonObject({{QStringLiteral("fielderrors"), vr.errorsJsonObject()}}));
                return;
            }
        }
    }

    c->stash({
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guestgroups/add.tmpl")}
             });
}

void ControlCenterEvents::editGuestGroups(Context *c, const QString &id)
{
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/guestgroups/edit.tmpl")}
             });
}

void ControlCenterEvents::templates(Context *c)
{
    const Event event = Event::fromStash(c);

    Error e;
    const std::vector<InvitationTemplate> templates = InvitationTemplate::list(c, &e, event);

    QMap<QString,QString> templatesTableHeaders;
    //: table header
    templatesTableHeaders.insert(QStringLiteral("name"), c->translate("ControlCenterEvents", "name"));
    //: table header
    templatesTableHeaders.insert(QStringLiteral("type"), c->translate("ControlCenterEvents", "type"));
    //: table header
    templatesTableHeaders.insert(QStringLiteral("salutation"), c->translate("ControlCenterEvents", "salutation"));
    //: table header
    templatesTableHeaders.insert(QStringLiteral("notification"), c->translate("ControlCenterEvents", "notification"));
    //: table header
    templatesTableHeaders.insert(QStringLiteral("id"), c->translate("ControlCenterEvents", "ID"));

    c->stash({
                 {QStringLiteral("templates_table_headers"), QVariant::fromValue<QMap<QString,QString>>(templatesTableHeaders)},
                 {QStringLiteral("templates"), QVariant::fromValue<std::vector<InvitationTemplate>>(templates)},
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Templates")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/templates/index.tmpl")}
             });
}

void ControlCenterEvents::addTemplate(Context *c)
{
    InvitationTemplate::Type typeSelected = InvitationTemplate::Invalid;
    GuestGroup::Salutation salutationSelected = GuestGroup::SalutationInvalid;
    Guest::Notification notificationSelected = Guest::NotNotified;

    if (c->req()->isPost()) {
        static Validator v({
                               new ValidatorRequired(QStringLiteral("name")),
                               new ValidatorRequired(QStringLiteral("type")),
                               new ValidatorRequiredIf(QStringLiteral("subject"), QStringLiteral("notification"), QStringList({QString::number(static_cast<int>(Guest::Email)), QString::number(static_cast<int>(Guest::Postal))}), ValidatorMessages(QT_TRANSLATE_NOOP("ControlCenterEvents", "Subject"), QT_TRANSLATE_NOOP("ControlCenterEvents", "Please enter a subject if you have selected email or postal as the notification type."))),
                               new ValidatorIn(QStringLiteral("type"), InvitationTemplate::typeValues()),
                               new ValidatorBetween(QStringLiteral("type"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("salutation")),
                               new ValidatorIn(QStringLiteral("salutation"), GuestGroup::salutationValues()),
                               new ValidatorBetween(QStringLiteral("salutation"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("notification")),
                               new ValidatorIn(QStringLiteral("notification"), Guest::notificationValues()),
                               new ValidatorBetween(QStringLiteral("notification"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("text"))
                           });
        const ValidatorResult vr = v.validate(c, Validator::FillStashOnError|Validator::BodyParamsOnly);
        if (vr) {
            const Event event = Event::fromStash(c);
            Error e;
            const InvitationTemplate templ = InvitationTemplate::create(c, &e, event, vr.values());
            if (templ.isValid()) {
                c->res()->redirect(c->uriForAction(QStringLiteral("/controlcenter/events/templates"), QStringList(QString::number(event.id())), QStringList(), StatusMessage::statusQuery(c, c->translate("ControlCenterEvents", "Successfully created new template “%1” (ID: %2).").arg(templ.name(), QString::number(templ.id())))));
                return;
            } else {
                e.toStash(c, true);
                return;
            }
        } else {
            typeSelected = InvitationTemplate::typeStringToEnum(c->req()->bodyParam(QStringLiteral("type")));
            salutationSelected = GuestGroup::salutationStringToEnum(c->req()->bodyParam(QStringLiteral("salutation")));
            notificationSelected = Guest::notificationStringToEnum(c->req()->bodyParam(QStringLiteral("notification")));
        }
    }

    const std::vector<OptionItem> typeOptions = InvitationTemplate::typeOptionList(c, typeSelected);
    const std::vector<OptionItem> salutationOptions = GuestGroup::salutationOptionList(c, salutationSelected);
    const std::vector<OptionItem> notificationOptions = Guest::notificationOptionList(c, notificationSelected);

    c->stash({
                 {QStringLiteral("type_options"), QVariant::fromValue<std::vector<OptionItem>>(typeOptions)},
                 {QStringLiteral("salutation_options"), QVariant::fromValue<std::vector<OptionItem>>(salutationOptions)},
                 {QStringLiteral("notification_options"), QVariant::fromValue<std::vector<OptionItem>>(notificationOptions)},
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Add template")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/templates/add.tmpl")}
             });
}

void ControlCenterEvents::editTemplate(Context *c, const QString &id)
{
    bool ok = false;
    const int templateId = Utils::strToDbid(id, &ok, c->translate("ControlCenterEvents", "Invalid template database ID."), c);
    if (!ok) {
        return;
    }

    Error e;
    InvitationTemplate templ = InvitationTemplate::get(c, &e, templateId);
    if (e.type() != Error::NoError) {
        e.toStash(c, true);
        return;
    }

    InvitationTemplate::Type typeSelected         = templ.type();
    GuestGroup::Salutation   salutationSelected   = templ.salutation();
    Guest::Notification      notificationSelected = templ.notification();

    if (c->req()->isPost()) {
        static Validator v({
                               new ValidatorRequired(QStringLiteral("name")),
                               new ValidatorRequired(QStringLiteral("type")),
                               new ValidatorRequiredIf(QStringLiteral("subject"), QStringLiteral("notification"), QStringList({QString::number(static_cast<int>(Guest::Email)), QString::number(static_cast<int>(Guest::Postal))}), ValidatorMessages(QT_TRANSLATE_NOOP("ControlCenterEvents", "Subject"), QT_TRANSLATE_NOOP("ControlCenterEvents", "Please enter a subject if you have selected email or postal as the notification type."))),
                               new ValidatorIn(QStringLiteral("type"), InvitationTemplate::typeValues()),
                               new ValidatorBetween(QStringLiteral("type"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("salutation")),
                               new ValidatorIn(QStringLiteral("salutation"), GuestGroup::salutationValues()),
                               new ValidatorBetween(QStringLiteral("salutation"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("notification")),
                               new ValidatorIn(QStringLiteral("notification"), Guest::notificationValues()),
                               new ValidatorBetween(QStringLiteral("notification"), QMetaType::Int, -127, 127),
                               new ValidatorRequired(QStringLiteral("text"))
                           });
        const ValidatorResult vr = v.validate(c, Validator::FillStashOnError|Validator::BodyParamsOnly);
        if (vr) {
            if (templ.update(c, &e, vr.values())) {

            } else {

            }
        } else {
            typeSelected         = InvitationTemplate::typeStringToEnum(c->req()->bodyParam(QStringLiteral("type")));
            salutationSelected   = GuestGroup::salutationStringToEnum(c->req()->bodyParam(QStringLiteral("salutation")));
            notificationSelected = Guest::notificationStringToEnum(c->req()->bodyParam(QStringLiteral("notification")));
        }

    }

    const std::vector<OptionItem> typeOptions           = InvitationTemplate::typeOptionList(c, typeSelected);
    const std::vector<OptionItem> salutationOptions     = GuestGroup::salutationOptionList(c, salutationSelected);
    const std::vector<OptionItem> notificationOptions   = Guest::notificationOptionList(c, notificationSelected);

    c->stash({
                 {QStringLiteral("templ"), QVariant::fromValue<InvitationTemplate>(templ)},
                 {QStringLiteral("type_options"), QVariant::fromValue<std::vector<OptionItem>>(typeOptions)},
                 {QStringLiteral("salutation_options"), QVariant::fromValue<std::vector<OptionItem>>(salutationOptions)},
                 {QStringLiteral("notification_options"), QVariant::fromValue<std::vector<OptionItem>>(notificationOptions)},
                 {QStringLiteral("site_subtitle"), c->translate("ControlCenterEvents", "Edit template")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/events/templates/edit.tmpl")}
             });
}

#include "moc_controlcenterevents.cpp"
