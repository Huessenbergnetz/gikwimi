/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_DATABASEREFRESHCOMMAND_H
#define GIKWIMICTL_DATABASEREFRESHCOMMAND_H

#include "databasecommand.h"
#include <QObject>

class DatabaseRefreshCommand : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseRefreshCommand(QObject *parent = nullptr);
    ~DatabaseRefreshCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_DATABASEREFRESHCOMMAND_H
