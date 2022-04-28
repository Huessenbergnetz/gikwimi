/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_DATABASEMIGRATIONCOMMAND_H
#define GIKWIMICTL_DATABASEMIGRATIONCOMMAND_H

#include "databasecommand.h"
#include <QObject>

class DatabaseMigrationCommand : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseMigrationCommand(QObject *parent = nullptr);
    ~DatabaseMigrationCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_DATABASEMIGRATIONCOMMAND_H
