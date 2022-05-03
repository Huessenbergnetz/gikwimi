/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_EVENTADDCOMMAND_H
#define GIKWIMICTL_EVENTADDCOMMAND_H

#include "../command.h"

class EventAddCommand : public Command
{
    Q_OBJECT
public:
    explicit EventAddCommand(QObject *parent = nullptr);
    ~EventAddCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_EVENTADDCOMMAND_H
