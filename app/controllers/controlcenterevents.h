/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_CONTROLCENTEREVENTS_H
#define GIKWIMI_CONTROLCENTEREVENTS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ControlCenterEvents : public Controller
{
    Q_OBJECT
    C_NAMESPACE("controlcenter/events")
public:
    explicit ControlCenterEvents(QObject *parent = nullptr);
    ~ControlCenterEvents() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *);

    C_ATTR(add, :Path("add") :Args(0))
    void add(Context *c);
};

#endif // CONTROLCENTEREVENTS_H
