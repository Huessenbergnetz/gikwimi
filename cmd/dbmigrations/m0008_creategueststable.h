/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0008_CREATEGUESTSTABLE_H
#define M0008_CREATEGUESTSTABLE_H

#include <Firfuorida/migration.h>

class M0008_CreateGuestsTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0008_CreateGuestsTable)
public:
    explicit M0008_CreateGuestsTable(Firfuorida::Migrator *parent);
    ~M0008_CreateGuestsTable() override;

    void up() override;
    void down() override;
};

#endif // M0008_CREATEGUESTSTABLE_H

