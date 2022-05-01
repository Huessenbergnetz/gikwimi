/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenteraddressbooks.h"
#include "global.h"
#include "../objects/user.h"
#include "../objects/addressbook.h"
#include "../objects/error.h"
#include "../objects/menuitem.h"

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

    buildMenu(c);

    QMap<QString,QString> addressbooksTableHeaders;
    //: table header
    addressbooksTableHeaders.insert(QStringLiteral("id"), c->translate("ControlCenterAddressBooks", "ID"));
    //: table header
    addressbooksTableHeaders.insert(QStringLiteral("type"), c->translate("ControlCenterAddressBooks", "type"));
    //: table header
    addressbooksTableHeaders.insert(QStringLiteral("name"), c->translate("ControlCenterAddressBooks", "name"));

    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Addressbooks")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbooks/index.tmpl")},
                 {QStringLiteral("addressbooks"), QVariant::fromValue<std::vector<AddressBook>>(addressBooks)},
                 {QStringLiteral("addressbooks_table_headers"), QVariant::fromValue<QMap<QString,QString>>(addressbooksTableHeaders)}
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

void ControlCenterAddressBooks::buildMenu(Context *c)
{
    std::vector<MenuItem> addressBooksMenu;
    addressBooksMenu.reserve(1);

    addressBooksMenu.emplace_back(c, QStringLiteral("addressbooks_menu_add"), c->translate("ControlCenterAddressBooks", "add"), QString(), QStringLiteral("add"), QStringLiteral("controlcenter/addressbooks"));

    c->setStash(QStringLiteral("controlcenter_addressbooks_menu"), QVariant::fromValue<std::vector<MenuItem>>(addressBooksMenu));
}
