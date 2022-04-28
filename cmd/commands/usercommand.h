/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_USERCOMMAND_H
#define GIKWIMICTL_USERCOMMAND_H

#include <command.h>
#include <QObject>

class UserCommand : public Command
{
    Q_OBJECT
public:
    explicit UserCommand(QObject *parent = nullptr);
    ~UserCommand();

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_USERCOMMAND_H
