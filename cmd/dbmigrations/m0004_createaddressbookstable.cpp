/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0004_createaddressbookstable.h"

M0004_CreateAddressBooksTable::M0004_CreateAddressBooksTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0004_CreateAddressBooksTable::~M0004_CreateAddressBooksTable()
{

}

void M0004_CreateAddressBooksTable::up()
{
    auto t = create(QStringLiteral("addressbooks"));
    t->increments();
    t->integer(QStringLiteral("user_id"))->unSigned();
    t->tinyInteger(QStringLiteral("type"))->unSigned()->defaultValue(1);
    t->varChar(QStringLiteral("name"), 63);
    t->text(QStringLiteral("settings"))->nullable();
    t->dateTime(QStringLiteral("created_at"));
    t->dateTime(QStringLiteral("updated_at"));
    t->bigInteger(QStringLiteral("locked_at"))->defaultValue(0)->nullable();
    t->integer(QStringLiteral("locked_by"))->unSigned()->defaultValue(0)->nullable();

    t->foreignKey(QStringLiteral("user_id"), QStringLiteral("users"), QStringLiteral("id"), QStringLiteral("addressbooks_user_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));

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

void M0004_CreateAddressBooksTable::down()
{
    drop(QStringLiteral("addressbooks"));
}

#include "moc_m0004_createaddressbookstable.cpp"

