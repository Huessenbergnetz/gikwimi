/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_CONTROLCENTER_H
#define GIKWIMI_CONTROLCENTER_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ControlCenter : public Controller
{
    Q_OBJECT
    C_NAMESPACE("controlcenter")
public:
    explicit ControlCenter(QObject *parent = nullptr);
    ~ControlCenter() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *c);

private:
    C_ATTR(Auto, :Private)
    bool Auto(Context *c);

    void buildMenu(Context *c);
};

#endif // GIKWIMI_CONTROLCENTER_H
