/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "root.h"
#include "logging.h"
#include "gikwimiconfig.h"

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
    qCDebug(GIK_CORE) << "Entering Root::Auto()";

    StatusMessage::load(c);

    c->setStash(QStringLiteral("site_name"), GikwimiConfig::siteName());

    return true;
}

#include "moc_root.cpp"
