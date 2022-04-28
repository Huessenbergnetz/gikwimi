/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_DATABASERESETCOMMAND_H
#define GIKWIMICTL_DATABASERESETCOMMAND_H

#include "databasecommand.h"
#include <QObject>

class DatabaseResetCommand : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseResetCommand(QObject *parent = nullptr);
    ~DatabaseResetCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_DATABASERESETCOMMAND_H
