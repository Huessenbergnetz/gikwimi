/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0004_CREATEADDRESSBOOKSTABLE_H
#define M0004_CREATEADDRESSBOOKSTABLE_H

#include <Firfuorida/migration.h>

class M0004_CreateAddressBooksTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0004_CreateAddressBooksTable)
public:
    explicit M0004_CreateAddressBooksTable(Firfuorida::Migrator *parent);
    ~M0004_CreateAddressBooksTable() override;

    void up() override;
    void down() override;
};

#endif // M0004_CREATEADDRESSBOOKSTABLE_H

