/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0009_CREATETEMPLATESTABLE_H
#define M0009_CREATETEMPLATESTABLE_H

#include <Firfuorida/migration.h>

class M0009_CreateTemplatesTable : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0009_CreateTemplatesTable)
public:
    explicit M0009_CreateTemplatesTable(Firfuorida::Migrator *parent);
    ~M0009_CreateTemplatesTable() override;

    void up() override;
    void down() override;
};

#endif // M0009_CREATETEMPLATESTABLE_H

