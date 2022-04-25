/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenteraddressbook.h"

ControlCenterAddressBook::ControlCenterAddressBook(QObject *parent)
    : Controller{parent}
{

}

ControlCenterAddressBook::~ControlCenterAddressBook() = default;

void ControlCenterAddressBook::index(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Addressbook")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbook/index.tmpl")}
             });
}

void ControlCenterAddressBook::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterAddressBook", "Add Contact")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/addressbook/add.tmpl")}
             });
}
