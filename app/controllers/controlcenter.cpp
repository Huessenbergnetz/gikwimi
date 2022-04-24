/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controlcenter.h"
#include "logging.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>

ControlCenter::ControlCenter(QObject *parent)
    : Controller{parent}
{

}

ControlCenter::~ControlCenter() = default;

void ControlCenter::index(Context *c)
{
    QString body;
    body = QStringLiteral("Controlcenter!");
    c->response()->setBody(body);
    c->response()->setContentType(QLatin1String("text/html; charset=utf-8"));
}

bool ControlCenter::Auto(Context *c)
{
    qCDebug(GIK_CORE) << "Entering ControlCenter::Auto()";

    const AuthenticationUser user = Authentication::user(c);

    if (Q_UNLIKELY(user.isNull())) {
        ParamsMultiMap query;
        query.insert(QStringLiteral("redirect_to"), QString::fromLatin1(c->req()->uri().toEncoded()));
        c->res()->redirect(c->uriFor(QStringLiteral("/login"), QStringList(), query));
        return false;
    }

    return true;
}
