/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0005_createcontactstable.h"

M0005_CreateContactsTable::M0005_CreateContactsTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0005_CreateContactsTable::~M0005_CreateContactsTable()
{

}

void M0005_CreateContactsTable::up()
{
    auto t = create(QStringLiteral("contacts"));
    t->increments();
    t->integer(QStringLiteral("addressbook_id"))->unSigned();
    t->varChar(QStringLiteral("uid"))->nullable();
    t->varChar(QStringLiteral("given_name"));
    t->varChar(QStringLiteral("family_name"));
    t->longText(QStringLiteral("vcard"));

    t->foreignKey(QStringLiteral("addressbook_id"), QStringLiteral("addressbooks"), QStringLiteral("id"), QStringLiteral("addresses_books_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onDelete(QStringLiteral("CASCADE"));

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

void M0005_CreateContactsTable::down()
{
    drop(QStringLiteral("contacts"));
}

#include "moc_m0005_createcontactstable.cpp"

