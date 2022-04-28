/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_USERADDCOMMAND_H
#define GIKWIMICTL_USERADDCOMMAND_H

#include <command.h>
#include <QObject>

class UserAddCommand : public Command
{
    Q_OBJECT
public:
    explicit UserAddCommand(QObject *parent = nullptr);
    ~UserAddCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_USERADDCOMMAND_H
