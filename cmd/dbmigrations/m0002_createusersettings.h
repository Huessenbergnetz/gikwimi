/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0002_CREATEUSERSETTINGS_H
#define M0002_CREATEUSERSETTINGS_H

#include <Firfuorida/migration.h>

class M0002_CreateUserSettings : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0002_CreateUserSettings)
public:
    explicit M0002_CreateUserSettings(Firfuorida::Migrator *parent);
    ~M0002_CreateUserSettings() override;

    void up() override;
    void down() override;
};

#endif // M0002_CREATEUSERSETTINGS_H

