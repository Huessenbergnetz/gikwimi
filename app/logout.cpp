/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "logout.h"
#include "logging.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationuser.h>
#include <Cutelyst/Plugins/Session/Session>

Logout::Logout(QObject *parent)
    : Controller{parent}
{

}

Logout::~Logout() = default;

void Logout::index(Context *c)
{
    Authentication *auth = c->plugin<Authentication*>();

    const QString username = auth->user(c).value(QStringLiteral("username")).toString();
    auth->logout(c);

    qCInfo(GIK_AUTH) << "User" << username << "logged out";

    Session::deleteSession(c, QStringLiteral("User logged out"));

    c->res()->redirect(c->uriFor(QStringLiteral("/")));
}

#include "moc_logout.cpp"
