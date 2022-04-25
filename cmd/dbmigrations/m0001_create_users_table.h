/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M20220413T113135_CREATE_USERS_TABLE_H
#define M20220413T113135_CREATE_USERS_TABLE_H

#include <Firfuorida/Migration>

class M0001_Create_Users_Table : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0001_Create_Users_Table)
public:
    explicit M0001_Create_Users_Table(Firfuorida::Migrator *parent);
    ~M0001_Create_Users_Table() override;

    void up() override;
    void down() override;
};

#endif // M20220413T113135_CREATE_USERS_TABLE_H

