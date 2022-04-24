/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenterusers.h"
#include "objects/menuitem.h"
#include "logging.h"

ControlCenterUsers::ControlCenterUsers(QObject *parent)
    : Controller{parent}
{

}

ControlCenterUsers::~ControlCenterUsers() = default;

void ControlCenterUsers::index(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterUsers", "Users")}
             });
}

void ControlCenterUsers::add(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterUsers", "Add User")}
             });
}

bool ControlCenterUsers::Auto(Context *c)
{
    qCDebug(GIK_CORE) << "Entering ControlCenterUsers::Auto()";

    buildMenu(c);

    return true;
}

void ControlCenterUsers::buildMenu(Context *c)
{
    const auto mainMenu = c->stash(QStringLiteral("controlcenter_main_menu")).value<std::vector<MenuItem>>();
    std::vector<MenuItem> newMainMenu;
    for (const MenuItem &item : mainMenu) {
        if (item.name() == QLatin1String("controlcenter_users")) {
            MenuItem newItem = item;
            newItem.addChildItem(MenuItem(c, QStringLiteral("controlcenter_users_add"), c->translate("ControlCenterUsers", "Add"), QString(), QStringLiteral("add"), QStringLiteral("controlcenter/users")));
            newMainMenu.push_back(newItem);
        } else {
            newMainMenu.push_back(item);
        }
    }
    c->setStash(QStringLiteral("controlcenter_main_menu"), QVariant::fromValue<std::vector<MenuItem>>(newMainMenu));
}

#include "moc_controlcenterusers.cpp"
