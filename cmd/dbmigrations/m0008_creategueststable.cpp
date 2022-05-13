/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0008_creategueststable.h"

M0008_CreateGuestsTable::M0008_CreateGuestsTable(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M0008_CreateGuestsTable::~M0008_CreateGuestsTable()
{

}

void M0008_CreateGuestsTable::up()
{
    auto t = create(QStringLiteral("guests"));
    t->increments();
    t->charCol(QStringLiteral("uid"), 6)->unique();
    t->integer(QStringLiteral("group_id"))->unSigned();
    t->integer(QStringLiteral("contact_id"))->unSigned();
    t->varChar(QStringLiteral("partner_given_name"), 63)->nullable();
    t->varChar(QStringLiteral("partner_family_name"), 63)->nullable();
    t->tinyInteger(QStringLiteral("adults"))->unSigned()->defaultValue(1);
    t->tinyInteger(QStringLiteral("adults_accepted"))->unSigned()->defaultValue(0);
    t->tinyInteger(QStringLiteral("children"))->unSigned()->defaultValue(0);
    t->tinyInteger(QStringLiteral("children_accepted"))->unSigned()->defaultValue(0);
    t->tinyInteger(QStringLiteral("status"))->defaultValue(0);
    t->tinyInteger(QStringLiteral("notified"))->unSigned()->defaultValue(0);
    t->text(QStringLiteral("note"))->nullable();
    t->text(QStringLiteral("comment"))->nullable();
    t->tinyInteger(QStringLiteral("salutation"))->defaultValue(0);
    t->dateTime(QStringLiteral("created_at"));
    t->dateTime(QStringLiteral("updated_at"));
    t->bigInteger(QStringLiteral("locked_at"))->defaultValue(0)->nullable();
    t->integer(QStringLiteral("locked_by"))->unSigned()->defaultValue(0)->nullable();

    t->foreignKey(QStringLiteral("group_id"), QStringLiteral("guestgroups"), QStringLiteral("id"), QStringLiteral("guests_guestgroups_id_idx"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));
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

void M0008_CreateGuestsTable::down()
{
    drop(QStringLiteral("guests"));
}

#include "moc_m0008_creategueststable.cpp"

