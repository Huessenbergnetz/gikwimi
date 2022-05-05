/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0007_creategueststable.h"

M0007_CreateGuestsTable::M0007_CreateGuestsTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0007_CreateGuestsTable::~M0007_CreateGuestsTable()
{

}

void M0007_CreateGuestsTable::up()
{
    auto t = create(QStringLiteral("guests"));
    t->increments();
    t->integer(QStringLiteral("event_id"))->unSigned();
    t->integer(QStringLiteral("contact_id"))->unSigned();
    t->integer(QStringLiteral("partner_id"))->unSigned();
    t->varChar(QStringLiteral("partner_given_name"), 63);
    t->varChar(QStringLiteral("partner_family_name"), 63);
    t->tinyInteger(QStringLiteral("adults"))->unSigned()->defaultValue(1);
    t->tinyInteger(QStringLiteral("children"))->unSigned()->defaultValue(0);
    t->tinyInteger(QStringLiteral("status"))->defaultValue(0);
    t->tinyInteger(QStringLiteral("notified"))->unSigned()->defaultValue(0);

    t->foreignKey(QStringLiteral("event_id"), QStringLiteral("events"), QStringLiteral("id"), QStringLiteral("guests_event_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));
    t->foreignKey(QStringLiteral("contact_id"), QStringLiteral("contacts"), QStringLiteral("id"), QStringLiteral("guests_contact_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));

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

void M0007_CreateGuestsTable::down()
{
    drop(QStringLiteral("guests"));
}

#include "moc_m0007_creategueststable.cpp"

