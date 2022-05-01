/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenteraddressbooks.h"
#include "global.h"
#include "../objects/user.h"
#include "../objects/addressbook.h"
#include "../objects/error.h"

#include <Cutelyst/Plugins/Authentication/authenticationuser.h>

ControlCenterAddressBooks::ControlCenterAddressBooks(QObject *parent)
    : Controller{parent}
{

}

ControlCenterAddressBooks::~ControlCenterAddressBooks() = default;

void ControlCenterAddressBooks::index(Context *c)
{
    const User currentUser = User::fromStash(c);

    Error e;
    const std::vector<AddressBook> addressBooks = AddressBook::list(c, e, currentUser);

    if (e.type() != Error::NoError) {
        e.toStash(c, true);
        return;
    }

    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Addressbook")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbooks/index.tmpl")},
                 {QStringLiteral("addressBooks"), QVariant::fromValue<std::vector<AddressBook>>(addressBooks)}
             });
}

void ControlCenterAddressBooks::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Add addressbook")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbooks/add.tmpl")}
             });
}

void ControlCenterAddressBooks::edit(Context *c, const QString &id)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Edit addressbook")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbooks/edit.tmpl")}
             });
}
