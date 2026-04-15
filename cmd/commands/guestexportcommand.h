/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_GUESTEXPORTCOMMAND_H
#define GIKWIMICTL_GUESTEXPORTCOMMAND_H

#include "command.h"

class GuestExportCommand : public Command
{
    Q_OBJECT
    Q_DISABLE_COPY(GuestExportCommand)
public:
    explicit GuestExportCommand(QObject *parent = nullptr);
    ~GuestExportCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_GUESTEXPORTCOMMAND_H
