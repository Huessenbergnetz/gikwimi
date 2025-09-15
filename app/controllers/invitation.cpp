/*
 * SPDX-FileCopyrightText: (C) 2022, 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "invitation.h"
#include "../objects/guest.h"
#include "../objects/error.h"
#include "../gikwimiconfig.h"
#include "../logging.h"

#include <Cutelyst/Plugins/Utils/Validator> // includes the main validator
#include <Cutelyst/Plugins/Utils/ValidatorResult> // includes the validator result
#include <Cutelyst/Plugins/Utils/validatorrequired.h>
#include <Cutelyst/Plugins/Utils/validatorbetween.h>
#include <Cutelyst/Plugins/Utils/validatorin.h>
#include <Cutelyst/Plugins/Utils/validatorrequiredif.h>
#include <Cutelyst/Plugins/Utils/validatorinteger.h>
#include <Cutelyst/Plugins/StatusMessage>

#include <SimpleMail/SimpleMail>

using namespace Qt::StringLiterals;

Invitation::Invitation(QObject *parent)
    : Controller{parent}
{

}

Invitation::~Invitation() = default;

void Invitation::index(Context *c, const QString &uid)
{
    Error e;
    Guest guest = Guest::getByUid(c, &e, uid);
    if (e.type() == Error::NoError) {
        c->setStash(QStringLiteral("guest"), QVariant::fromValue<Guest>(guest));
    } else {
        e.toStash(c);
    }

    const Event event = guest.group().event();

    Guest::Status consent;
    if (guest.status() == Guest::DefaultStaus) {
        if (event.participation() == Event::Refusal) {
            consent = Guest::Agreed;
        } else {
            consent = Guest::Canceled;
        }
    } else {
        consent = guest.status();
    }

    if (c->req()->isPost()) {
        static Validator v({
                               new ValidatorRequired(QStringLiteral("consent")),
                               new ValidatorIn(QStringLiteral("consent"), Guest::statusValues()),
                               new ValidatorBetween(QStringLiteral("consent"), QMetaType::Int, static_cast<int>(Guest::Agreed), static_cast<int>(Guest::Canceled)),
                               new ValidatorRequiredIf(QStringLiteral("adults"), QStringLiteral("consent"), QStringList(QString::number(static_cast<int>(Guest::Agreed)))),
                               new ValidatorInteger(QStringLiteral("adults"), QMetaType::UInt),
                               new ValidatorRequiredIf(QStringLiteral("children"), QStringLiteral("consent"), QStringList(QString::number(static_cast<int>(Guest::Agreed)))),
                               new ValidatorInteger(QStringLiteral("children"), QMetaType::UInt)
                           });
        const ValidatorResult vr = v.validate(c, Validator::BodyParamsOnly);
        if (vr) {
            QVariantHash values = vr.values();
            values.insert(QStringLiteral("comment"), c->req()->bodyParam(QStringLiteral("comment")));
            if (guest.updateStatus(c, &e, values)) {
                QString statusMessage = guest.adults() > 1 ? QStringLiteral("Vielen Dank für Eure Rückmeldung!") : QStringLiteral("Vielen Dank für Deine Rückmeldung!");
                statusMessage += QChar(QChar::Space);
                if (guest.status() == Guest::Agreed) {
                    statusMessage += QStringLiteral("Schön dass wir uns am 25. Juni ab 11 Uhr in Eissen bei mir im Garten sehen. 🙂");
                } else {
                    statusMessage += QStringLiteral("Schade, dass wir uns am 25. Juni nicht sehen. 🙁");
                }
                c->setStash(QStringLiteral("status_msg"), statusMessage);
                c->setStash(QStringLiteral("guest"), QVariant::fromValue<Guest>(guest));
                consent = guest.status();

                sendNotificationEmails(c, guest, event);
            }
        }
    }

    c->res()->setHeader("X-Robots-Tag"_ba, "none"_ba);

    c->stash({   {QStringLiteral("consent"), static_cast<int>(consent)},
                 {QStringLiteral("site_title"), event.title()},
                 {QStringLiteral("no_wrapper"), true},
                 {QStringLiteral("template"), QStringLiteral("invitation.tmpl")},
             });
}

void Invitation::sendNotificationEmails(Context *c, const Guest &guest, const Event &event)
{
    if (GikwimiConfig::emailFromMail().isEmpty()) {
        qCWarning(GIK_CORE) << "Will not send notification email: empty from mail";
        return;
    }

    if (GikwimiConfig::emailHost().isEmpty()) {
        qCWarning(GIK_CORE) << "Will not send notification email: empty mail host";
        return;
    }

    const QString guestString = guest.partnerGivenName().isEmpty() ? guest.givenName() : c->translate("Invitation", "%1 and %2").arg(guest.givenName(), guest.partnerGivenName());

    auto server = new SimpleMail::Server;
    server->setHost(GikwimiConfig::emailHost());
    server->setPort(GikwimiConfig::emailPort());
    server->setConnectionType(GikwimiConfig::emailConnectionType());
    server->setUsername(GikwimiConfig::emailUser());
    server->setPassword(GikwimiConfig::emailPassword());

    SimpleMail::MimeMessage message;
    message.setSender(SimpleMail::EmailAddress(GikwimiConfig::emailFromMail(), GikwimiConfig::emailFromName()));
    message.addTo(SimpleMail::EmailAddress(event.user().email()));
    QString subject;
    if (guest.partnerGivenName().isEmpty() && guest.status() == Guest::Agreed) {
        subject = QStringLiteral("👍 ") + c->translate("Invitation", "%1: %2 has agreed").arg(event.title(), guestString);
    } else if (!guest.partnerGivenName().isEmpty() && guest.status() == Guest::Agreed) {
        subject = QStringLiteral("👍 ") + c->translate("Invitation", "%1: %2 have agreed").arg(event.title(), guestString);
    } else if (guest.partnerGivenName().isEmpty() && guest.status() == Guest::Canceled) {
        subject = QStringLiteral("👎 ") + c->translate("Invitation", "%1: %2 has canceled").arg(event.title(), guestString);
    } else if (!guest.partnerGivenName().isEmpty() && guest.status() == Guest::Canceled) {
        subject = QStringLiteral("👎 ") + c->translate("Invitation", "%1: %2 have canceled").arg(event.title(), guestString);
    }
    message.setSubject(subject);

    auto text = new SimpleMail::MimeText;
    if (guest.status() == Guest::Agreed) {
        text->setText(c->translate("Invitation", "Hello,\n\na guest of your event “%1” has accepted.\n\nGuest: %2\nAdults: %3\nChildren: %4\n\nComment:\n%5\n\nAutomatically sent by Gikwimi on %6").arg(event.title(), guestString, QString::number(guest.adultsAccepted()), QString::number(guest.childrenAccepted()), guest.comment(), c->uriFor(QStringLiteral("/")).toString()));
    } else {
        text->setText(c->translate("Invitation", "Hello,\n\na guest of your event “%1” has unfortunately cancelled.\n\nGuest: %2\nAdults: %3\nChildren: %4\n\nComment\n%5\n\nAutomatically sent by Gikwimi on %6").arg(event.title(), guestString, QString::number(guest.adults()), QString::number(guest.children()), guest.comment(), c->uriFor(QStringLiteral("/")).toString()));
    }

    // message.addPart(text);

    SimpleMail::ServerReply *reply = server->sendMail(message);
    connect(reply, &SimpleMail::ServerReply::finished, [reply]() {
        if (reply->error()) {
            qCCritical(GIK_CORE) << "Failed to send notification email:" << reply->responseCode() << reply->responseText();
        }
        reply->deleteLater();
    });
}

#include "moc_invitation.cpp"
