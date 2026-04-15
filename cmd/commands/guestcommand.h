/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_GUESTCOMMAND_H
#define GIKWIMICTL_GUESTCOMMAND_H

#include "command.h"
#include <QObject>

class GuestCommand : public Command
{
    Q_OBJECT
    Q_DISABLE_COPY(GuestCommand)
public:
    explicit GuestCommand(QObject *parent = nullptr);
    ~GuestCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_GUESTCOMMAND_H
