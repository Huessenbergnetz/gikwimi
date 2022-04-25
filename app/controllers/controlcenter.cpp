/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenter.h"
#include "logging.h"
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
                  {QStringLiteral("site_title"), c->translate("ControlCenter", "Dashboard")}
             });
}

bool ControlCenter::Auto(Context *c)
{
    qCDebug(GIK_CORE) << "Entering ControlCenter::Auto()";

    const AuthenticationUser user = Authentication::user(c);

    if (Q_UNLIKELY(user.isNull())) {
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
    std::vector<MenuItem> mainMenu({
                                       MenuItem(c, QStringLiteral("controlcenter_index"), c->translate("ControlCenter", "Dashboard"), QStringLiteral(R"(<i class="bi bi-speedometer2"></i>)"), QStringLiteral("index"), QStringLiteral("controlcenter")),
                                       MenuItem(c, QStringLiteral("controlcenter_users"), c->translate("ControlCenter", "Users"), QStringLiteral(R"(<i class="bi bi-people"></i>)"), QStringLiteral("index"), QStringLiteral("controlcenter/users")),
                                       MenuItem(c, QStringLiteral("controlcenter_settings"), c->translate("ControlCenter", "Settings"), QStringLiteral(R"(<i class="bi bi-sliders"></i>)"), QStringLiteral("index"), QStringLiteral("controlcenter/settings")),
                                   });
    c->setStash(QStringLiteral("controlcenter_main_menu"), QVariant::fromValue<std::vector<MenuItem>>(mainMenu));
}