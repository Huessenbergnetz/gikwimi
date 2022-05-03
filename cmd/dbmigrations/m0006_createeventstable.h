/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0006_CREATEEVENTSTABLE_H
#define M0006_CREATEEVENTSTABLE_H

#include <Firfuorida/migration.h>

class M0006_CreateEventsTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0006_CreateEventsTable)
public:
    explicit M0006_CreateEventsTable(Firfuorida::Migrator *parent);
    ~M0006_CreateEventsTable() override;

    void up() override;
    void down() override;
};

#endif // M0006_CREATEEVENTSTABLE_H

