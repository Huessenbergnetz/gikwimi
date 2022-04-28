/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_ADDRESSBOOKADDCOMMAND_H
#define GIKWIMICTL_ADDRESSBOOKADDCOMMAND_H

#include <command.h>
#include <QObject>

class AddressBookAddCommand : public Command
{
    Q_OBJECT
public:
    explicit AddressBookAddCommand(QObject *parent = nullptr);
    ~AddressBookAddCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_ADDRESSBOOKADDCOMMAND_H
