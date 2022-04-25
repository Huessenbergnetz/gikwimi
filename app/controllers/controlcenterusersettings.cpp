/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenterusersettings.h"

ControlCenterUserSettings::ControlCenterUserSettings(QObject *parent)
    : Controller{parent}
{

}

ControlCenterUserSettings::~ControlCenterUserSettings() = default;

void ControlCenterUserSettings::index(Context *c)
{
    c->stash({
                 {QStringLiteral("site_title"), c->translate("ControlCenterUsersettings", "User Settings")},
                 {QStringLiteral("template"), QStringLiteral("controlcenter/usersettings/index.tmpl")}
             });
}

#include "moc_controlcenterusersettings.cpp"
