/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcentersettings.h"

ControlCenterSettings::ControlCenterSettings(QObject *parent)
    : Controller{parent}
{

}

ControlCenterSettings::~ControlCenterSettings() = default;

void ControlCenterSettings::index(Context *c)
{
    c->response()->setBody(QStringLiteral("ControlCenter Settings"));
    c->response()->setContentType(QLatin1String("text/html; charset=utf-8"));
}

#include "moc_controlcentersettings.cpp"
