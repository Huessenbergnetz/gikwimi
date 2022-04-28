/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0003_createoptionstable.h"

M0003_CreateOptionsTable::M0003_CreateOptionsTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0003_CreateOptionsTable::~M0003_CreateOptionsTable()
{

}

void M0003_CreateOptionsTable::up()
{
    auto t = create(QStringLiteral("options"));
    t->increments(QStringLiteral("option_id"));
    t->varChar(QStringLiteral("option_name"), 127)->unique()->charset(QStringLiteral("latin1"));
    t->longText(QStringLiteral("option_value"));

    switch(dbType()) {
    case Firfuorida::Migrator::MariaDB:
    case Firfuorida::Migrator::MySQL:
        t->setEngine(QStringLiteral("InnoDB"));
        t->setCharset(QStringLiteral("utf8mb4"));
        t->setCollation(QStringLiteral("utf8mb4_unicode_ci"));
        break;
    case Firfuorida::Migrator::DB2:
    case Firfuorida::Migrator::InterBase:
    case Firfuorida::Migrator::ODBC:
    case Firfuorida::Migrator::OCI:
    case Firfuorida::Migrator::PSQL:
    case Firfuorida::Migrator::SQLite:
    case Firfuorida::Migrator::Invalid:
        break;
    }
}

void M0003_CreateOptionsTable::down()
{
    drop(QStringLiteral("options"));
}

#include "moc_m0003_createoptionstable.cpp"

