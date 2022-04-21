/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_LOGIN_H
#define GIKWIMI_LOGIN_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Login : public Controller
{
    Q_OBJECT
public:
    explicit Login(QObject *parent = nullptr);
    ~Login() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *c);
};

#endif // GIKWIMI_LOGIN_H
