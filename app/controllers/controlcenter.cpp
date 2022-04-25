/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenter.h"
#include "logging.h"
#include "utils.h"
#include "objects/menuitem.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>

ControlCenter::ControlCenter(QObject *parent)
    : Controller{parent}
{

}

ControlCenter::~ControlCenter() = default;

void ControlCenter::index(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenter", "Dashboard")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/index.tmpl")}
             });
}

bool ControlCenter::Auto(Context *c)
{
    qCDebug(GIK_CORE) << "Entering ControlCenter::Auto()";

    if (Q_UNLIKELY(!Authentication::userExists(c))) {
        ParamsMultiMap query;
        query.insert(QStringLiteral("redirect_to"), QString::fromLatin1(c->req()->uri().toEncoded()));
        c->res()->redirect(c->uriFor(QStringLiteral("/login"), QStringList(), query));
        return false;
    }

    buildMenu(c);

    return true;
}

void ControlCenter::buildMenu(Context *c)
{
    std::vector<MenuItem> mainMenu;
    mainMenu.reserve(3);

    mainMenu.emplace_back(c, QStringLiteral("controlcenter_index"), c->translate("ControlCenter", "Dashboard"), QStringLiteral(R"(<i class="bi bi-speedometer2 me-2"></i>)"), QStringLiteral("index"), QStringLiteral("controlcenter"));

    if (Utils::isAdmin(c)) {

        MenuItem usersMenu(c, QStringLiteral("controlcenter_users"), c->translate("ControlCenter", "Users"), QStringLiteral(R"(<i class="bi bi-people me-2"></i>)"), QStringLiteral("index"), QStringLiteral("controlcenter/users"));
        usersMenu.addChildItem(MenuItem(c, QStringLiteral("controlcenter_users_add"), c->translate("ControlCenterUsers", "Add"), QString(), QStringLiteral("add"), QStringLiteral("controlcenter/users")));
        mainMenu.push_back(usersMenu);

        mainMenu.emplace_back(c, QStringLiteral("controlcenter_settings"), c->translate("ControlCenter", "Settings", "general settings"), QStringLiteral(R"(<i class="bi bi-sliders me-2"></i>)"), QStringLiteral("index"), QStringLiteral("controlcenter/settings"));

    }

    c->setStash(QStringLiteral("controlcenter_main_menu"), QVariant::fromValue<std::vector<MenuItem>>(mainMenu));

    std::vector<MenuItem> userMenu;
    userMenu.reserve(2);

    userMenu.emplace_back(c, QStringLiteral("user_menu_settings"), c->translate("ControlCenter", "Settings", "user settings"), QString(), QStringLiteral("index"), QStringLiteral("controlcenter/usersettings"));
    userMenu.emplace_back(c, QStringLiteral("user_menu_logout"), c->translate("ControlCenter", "Logout"), QString(), QStringLiteral("index"), QStringLiteral("logout"));

    c->setStash(QStringLiteral("controlcenter_user_menu"), QVariant::fromValue<std::vector<MenuItem>>(userMenu));
}
