/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0001_create_users_table.h"

M0001_Create_Users_Table::M0001_Create_Users_Table(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0001_Create_Users_Table::~M0001_Create_Users_Table()
{

}

void M0001_Create_Users_Table::up()
{
    auto t = create(QStringLiteral("users"));

    t->increments();
    t->tinyInteger(QStringLiteral("type"))->defaultValue(0);
    t->varChar(QStringLiteral("username"))->unique();
    t->varChar(QStringLiteral("password"))->charset(QStringLiteral("latin1"));
    t->varChar(QStringLiteral("email"))->unique();
    t->dateTime(QStringLiteral("created_at"));
    t->dateTime(QStringLiteral("updated_at"));
    t->bigInteger(QStringLiteral("locked_at"))->defaultValue(0)->nullable();
    t->integer(QStringLiteral("locked_by"))->unSigned()->defaultValue(0)->nullable();

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

void M0001_Create_Users_Table::down()
{
    drop(QStringLiteral("users"));
}

#include "moc_m0001_create_users_table.cpp"

