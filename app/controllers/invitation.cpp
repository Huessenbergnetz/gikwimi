/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "invitation.h"

Invitation::Invitation(QObject *parent)
    : Controller{parent}
{

}

Invitation::~Invitation() = default;

void Invitation::index(Context *c, const QString &uid)
{

}

#include "moc_invitation.cpp"
