/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_CONTROLCENTERUSERS_H
#define GIKWIMI_CONTROLCENTERUSERS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ControlCenterUsers : public Controller
{
    Q_OBJECT
    C_NAMESPACE("controlcenter/users")
public:
    explicit ControlCenterUsers(QObject *parent = nullptr);
    ~ControlCenterUsers() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *c);

    C_ATTR(add, :Path("add") :Args(0))
    void add(Context *c);
};

#endif // GIKWIMI_CONTROLCENTERUSERS_H
