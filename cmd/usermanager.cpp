/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "usermanager.h"

UserManager::UserManager(const QString &iniPath, bool quiet)
    : Database(iniPath, quiet)
{

}

UserManager::~UserManager() = default;
