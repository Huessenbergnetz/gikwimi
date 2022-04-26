/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "utils.h"
#include "objects/user.h"
#include <QDebug>

#include <Cutelyst/Context>

bool Utils::isCurrentUserAdmin(Cutelyst::Context *c)
{
    return User::fromStash(c).isAdmin();
}
