/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "login.h"

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

    }

    c->setStash(QStringLiteral("username"), username);
    c->setStash(QStringLiteral("no_wrapper"), QStringLiteral("1"));
    c->setStash(QStringLiteral("template"), QStringLiteral("login.tmpl"));
}

#include "moc_login.cpp"
