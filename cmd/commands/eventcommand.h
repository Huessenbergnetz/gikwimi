/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICT_EVENTCOMMAND_H
#define GIKWIMICT_EVENTCOMMAND_H

#include "../command.h"
#include <QObject>

class EventCommand : public Command
{
    Q_OBJECT
public:
    explicit EventCommand(QObject *parent = nullptr);
    ~EventCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICT_EVENTCOMMAND_H
