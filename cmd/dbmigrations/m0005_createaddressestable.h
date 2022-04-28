/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0005_CREATEADDRESSESTABLE_H
#define M0005_CREATEADDRESSESTABLE_H

#include <Firfuorida/migration.h>

class M0005_CreateAddressesTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0005_CreateAddressesTable)
public:
    explicit M0005_CreateAddressesTable(Firfuorida::Migrator *parent);
    ~M0005_CreateAddressesTable() override;

    void up() override;
    void down() override;
};

#endif // M0005_CREATEADDRESSESTABLE_H

