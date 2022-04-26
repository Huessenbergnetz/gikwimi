/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "root.h"
#include "logging.h"
#include "gikwimiconfig.h"
#include "objects/error.h"
#include "objects/user.h"

#include <Cutelyst/Plugins/StatusMessage>
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationuser.h>

#include <QJsonObject>
#include <QJsonValue>

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

void Root::error(Context *c)
{
    const auto e = Error::fromStash(c);
    QString error_text, error_title;

    if (e.type() == Error::NoError) {
        switch(c->res()->status()) {
        case 404:
            error_title = c->translate("Root", "Not found");
            error_text  = c->translate("Root", "The requested resource could not be found or the requested page is not available.");
            break;
        case 403:
            error_title = c->translate("Root", "Access denied");
            error_text  = c->translate("Root", "You are not authorized to access this resource or to perform this action.");
            break;
        default:
            error_title = c->translate("Root", "Unknown error");
            error_text  = c->translate("Root", "Sorry, but an unknown error occurd while processing your request.");
            break;
        }
    } else {
        c->res()->setStatus(e.status());
        error_title = e.title(c);
        error_text  = e.text();
    }

    if (c->req()->xhr()) {
        c->res()->setJsonObjectBody({{QStringLiteral("error_msg"), QJsonValue(error_text)}});
    } else {
        const QString siteTitle = QString::number(c->res()->status()) + QStringLiteral(" – ") + error_title;
        c->stash({
                     {QStringLiteral("template"),       QStringLiteral("error.tmpl")},
                     {QStringLiteral("site_title"),     siteTitle},
                     {QStringLiteral("error_title"),    error_title},
                     {QStringLiteral("error_text"),     error_text},
                     {QStringLiteral("error_code"),     c->res()->status()}
                 });
    }
}

bool Root::Auto(Context *c)
{
    qCDebug(GIK_CORE) << "Entering Root::Auto()";

    StatusMessage::load(c);

    const AuthenticationUser authUser = Authentication::user(c);
    if (!authUser.isNull()) {
        User user(authUser);
        user.toStash(c);
    }

    c->setStash(QStringLiteral("site_name"), GikwimiConfig::siteName());

    return true;
}

#include "moc_root.cpp"
