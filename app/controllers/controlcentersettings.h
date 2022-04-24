/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_CONTROLCENTERSETTINGS_H
#define GIKWIMI_CONTROLCENTERSETTINGS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ControlCenterSettings : public Controller
{
    Q_OBJECT
    C_NAMESPACE("controlcenter/settings")
public:
    explicit ControlCenterSettings(QObject *parent = nullptr);
    ~ControlCenterSettings() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *);
};

#endif // GIKWIMI_CONTROLCENTERSETTINGS_H
