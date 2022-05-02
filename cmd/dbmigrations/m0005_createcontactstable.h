/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0005_CREATECONTACTSTABLE_H
#define M0005_CREATECONTACTSTABLE_H

#include <Firfuorida/migration.h>

class M0005_CreateContactsTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0005_CreateContactsTable)
public:
    explicit M0005_CreateContactsTable(Firfuorida::Migrator *parent);
    ~M0005_CreateContactsTable() override;

    void up() override;
    void down() override;
};

#endif // M0005_CREATECONTACTSTABLE_H

