/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "root.h"

#include <QCoreApplication>
#include <Cutelyst/Application>
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/StatusMessage>

using namespace Cutelyst;

Root::Root(QObject *parent) : Controller(parent)
{
}

Root::~Root()
{
}

void Root::index(Context *c)
{
    QString body;
    body = QStringLiteral("Hello World!");
    c->response()->setBody(body);
    c->response()->setContentType(QLatin1String("text/html; charset=utf-8"));
}

void Root::defaultPage(Context *c)
{
    c->response()->setBody(QStringLiteral("Page not found!"));
    c->response()->setStatus(404);
}

bool Root::Auto(Context *c)
{
    if (c->controllerName() == QLatin1String("Login")) {
        return true;
    }

    const AuthenticationUser user = Authentication::user(c);

    if (Q_UNLIKELY(user.isNull())) {
        c->res()->redirect(c->uriFor(QStringLiteral("/login")));
        return false;
    }

    StatusMessage::load(c);

    return true;
}

#include "moc_root.cpp"
