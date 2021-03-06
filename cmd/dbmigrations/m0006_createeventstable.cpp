/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0006_createeventstable.h"

M0006_CreateEventsTable::M0006_CreateEventsTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0006_CreateEventsTable::~M0006_CreateEventsTable()
{

}

void M0006_CreateEventsTable::up()
{
    auto t = create(QStringLiteral("events"));
    t->increments();
    t->integer(QStringLiteral("user_id"))->unSigned();
    t->varChar(QStringLiteral("title"), 127);
    t->varChar(QStringLiteral("slug"), 63)->charset(QStringLiteral("ascii"))->unique();
    t->dateTime(QStringLiteral("start_time"));
    t->dateTime(QStringLiteral("end_time"));
    t->varChar(QStringLiteral("timezone"), 63)->defaultValue(QStringLiteral("Europe/Berlin"));
    t->tinyInteger(QStringLiteral("audience"))->defaultValue(1);
    t->tinyInteger(QStringLiteral("participation"))->defaultValue(1);
    t->boolean(QStringLiteral("all_day"))->defaultValue(false);
    t->longText(QStringLiteral("description"))->nullable();
    t->text(QStringLiteral("settings"))->nullable();
    t->integer(QStringLiteral("default_group"))->nullable();
    t->dateTime(QStringLiteral("created_at"));
    t->dateTime(QStringLiteral("updated_at"));
    t->bigInteger(QStringLiteral("locked_at"))->defaultValue(0)->nullable();
    t->integer(QStringLiteral("locked_by"))->unSigned()->defaultValue(0)->nullable();

    t->foreignKey(QStringLiteral("user_id"), QStringLiteral("users"), QStringLiteral("id"), QStringLiteral("events_user_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));

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

void M0006_CreateEventsTable::down()
{
    drop(QStringLiteral("events"));
}

#include "moc_m0006_createeventstable.cpp"

