/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_CONTROLCENTERUSERSETTINGS_H
#define GIKWIMI_CONTROLCENTERUSERSETTINGS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ControlCenterUserSettings : public Controller
{
    Q_OBJECT
    C_NAMESPACE("controlcenter/usersettings")
public:
    explicit ControlCenterUserSettings(QObject *parent = nullptr);
    ~ControlCenterUserSettings() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *c);
};

#endif // CONTROLCENTERUSERSETTINGS_H
