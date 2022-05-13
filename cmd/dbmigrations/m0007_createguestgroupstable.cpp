/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0007_createguestgroupstable.h"

M0007_CreateGuestGroupsTable::M0007_CreateGuestGroupsTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0007_CreateGuestGroupsTable::~M0007_CreateGuestGroupsTable()
{

}

void M0007_CreateGuestGroupsTable::up()
{
    auto t = create(QStringLiteral("guestgroups"));
    t->increments();
    t->integer(QStringLiteral("event_id"))->unSigned();
    t->varChar(QStringLiteral("name"), 63);
    t->varChar(QStringLiteral("slug"), 63)->charset(QStringLiteral("ascii"))->unique();
    t->text(QStringLiteral("note"))->nullable();
    t->tinyInteger(QStringLiteral("salutation"));
    t->dateTime(QStringLiteral("created_at"));
    t->dateTime(QStringLiteral("updated_at"));
    t->bigInteger(QStringLiteral("locked_at"))->defaultValue(0)->nullable();
    t->integer(QStringLiteral("locked_by"))->unSigned()->defaultValue(0)->nullable();

    t->foreignKey(QStringLiteral("event_id"), QStringLiteral("events"), QStringLiteral("id"), QStringLiteral("guestgroups_event_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));

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

void M0007_CreateGuestGroupsTable::down()
{
    drop(QStringLiteral("guestgroups"));
}

#include "moc_m0007_createguestgroupstable.cpp"

