/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0007_CREATEGUESTGROUPSTABLE_H
#define M0007_CREATEGUESTGROUPSTABLE_H

#include <Firfuorida/migration.h>

class M0007_CreateGuestGroupsTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0007_CreateGuestGroupsTable)
public:
    explicit M0007_CreateGuestGroupsTable(Firfuorida::Migrator *parent);
    ~M0007_CreateGuestGroupsTable() override;

    void up() override;
    void down() override;
};

#endif // M0007_CREATEGUESTGROUPSTABLE_H

