/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0009_createtemplatestable.h"

M0009_CreateTemplatesTable::M0009_CreateTemplatesTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0009_CreateTemplatesTable::~M0009_CreateTemplatesTable()
{

}

void M0009_CreateTemplatesTable::up()
{
    auto t = create(QStringLiteral("templates"));
    t->increments();
    t->integer(QStringLiteral("event_id"))->unSigned();
    t->tinyInteger(QStringLiteral("type"));
    t->tinyInteger(QStringLiteral("salutation"));
    t->varChar(QStringLiteral("name"), 63);
    t->longText(QStringLiteral("text"));

    t->foreignKey(QStringLiteral("event_id"), QStringLiteral("events"), QStringLiteral("id"), QStringLiteral("templates_events_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));

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

void M0009_CreateTemplatesTable::down()
{
    drop(QStringLiteral("templates"));
}

#include "moc_m0009_createtemplatestable.cpp"

