/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "invitation.h"
#include "../objects/guest.h"
#include "../objects/error.h"

#include <Cutelyst/Plugins/Utils/Validator> // includes the main validator
#include <Cutelyst/Plugins/Utils/ValidatorResult> // includes the validator result
#include <Cutelyst/Plugins/Utils/validatorrequired.h>
#include <Cutelyst/Plugins/Utils/validatorbetween.h>
#include <Cutelyst/Plugins/Utils/validatorin.h>
#include <Cutelyst/Plugins/Utils/validatorrequiredif.h>
#include <Cutelyst/Plugins/Utils/validatorinteger.h>
#include <Cutelyst/Plugins/StatusMessage>

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
        const ValidatorResult vr = v.validate(c, Validator::FillStashOnError|Validator::BodyParamsOnly);
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
            }
        }
    }

    c->stash({   {QStringLiteral("consent"), static_cast<int>(consent)},
                 {QStringLiteral("site_title"), event.title()},
                 {QStringLiteral("no_wrapper"), true},
                 {QStringLiteral("template"), QStringLiteral("invitation.tmpl")},
             });
}

#include "moc_invitation.cpp"
