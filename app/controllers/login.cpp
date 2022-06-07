/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "login.h"
#include "logging.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationuser.h>
#include <Cutelyst/Plugins/Session/Session>

Q_LOGGING_CATEGORY(GIK_AUTH, "gikwimi.auth")

Login::Login(QObject *parent)
    : Controller{parent}
{

}

Login::~Login() = default;

void Login::index(Context *c)
{
    auto req = c->req();
    const QString username = req->bodyParam(QStringLiteral("username"));

    if (req->isPost()) {
        const QString password = req->bodyParam(QStringLiteral("password"));

        if (!username.isEmpty() && !password.isEmpty()) {
            if (Authentication::authenticate(c, req->bodyParams(), QStringLiteral("users"))) {
                qCInfo(GIK_AUTH, "User %s successfully logged in", qUtf8Printable(username));
                const QString redirectToQueryParam = req->queryParam(QStringLiteral("redirect_to"));
                const QUrl redirectToUrl = redirectToQueryParam.isEmpty() ? c->uriFor(QStringLiteral("/controlcenter")) : QUrl::fromEncoded(redirectToQueryParam.toLatin1());
                c->res()->redirect(redirectToUrl);
                return;
            } else {
                qCWarning(GIK_AUTH, "Bad username and/or password for user %s from IP %s", qUtf8Printable(username), qUtf8Printable(req->addressString()));
                c->setStash(QStringLiteral("error_msg"), c->translate("Login", "Arrrgh, bad username and/or password!"));
                c->res()->setStatus(Response::Forbidden);
            }
        }
    }

    c->res()->setHeader(QStringLiteral("X-Robots-Tag"), QStringLiteral("none"));

    c->stash({
                 {QStringLiteral("no_wrapper"), true},
                 {QStringLiteral("template"), QStringLiteral("login.tmpl")},
                 {QStringLiteral("site_title"), c->translate("Login", "Login", "site title")},
                 {QStringLiteral("login_field_label_username"), c->translate("Login", "Username")},
                 {QStringLiteral("login_field_label_password"), c->translate("Login", "Password")},
                 {QStringLiteral("login_button_label_login"), c->translate("Login", "Login", "button text")},
                 {QStringLiteral("username"), username}
             });
}

#include "moc_login.cpp"
