/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_DATABASEROLLBACKCOMMAND_H
#define GIKWIMI_DATABASEROLLBACKCOMMAND_H

#include "databasecommand.h"
#include <QObject>

class DatabaseRollbackCommand : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseRollbackCommand(QObject *parent = nullptr);
    ~DatabaseRollbackCommand();

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMI_DATABASEROLLBACKCOMMAND_H
