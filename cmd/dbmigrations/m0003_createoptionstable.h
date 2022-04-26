/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0003_CREATEOPTIONSTABLE_H
#define M0003_CREATEOPTIONSTABLE_H

#include <Firfuorida/migration.h>

class M0003_CreateOptionsTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0003_CreateOptionsTable)
public:
    explicit M0003_CreateOptionsTable(Firfuorida::Migrator *parent);
    ~M0003_CreateOptionsTable() override;

    void up() override;
    void down() override;
};

#endif // M0003_CREATEOPTIONSTABLE_H

