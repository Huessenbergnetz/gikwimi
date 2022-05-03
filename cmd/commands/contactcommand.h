/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_CONTACTCOMMAND_H
#define GIKWIMICTL_CONTACTCOMMAND_H

#include "../command.h"
#include <QObject>

class ContactCommand : public Command
{
    Q_OBJECT
public:
    explicit ContactCommand(QObject *parent = nullptr);
    ~ContactCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_CONTACTCOMMAND_H
