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

}

#include "moc_controlcentersettings.cpp"
