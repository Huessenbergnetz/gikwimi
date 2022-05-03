/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_ADDRESSBOOKCOMMAND_H
#define GIKWIMICTL_ADDRESSBOOKCOMMAND_H

#include "../command.h"
#include <QObject>

class AddressBookCommand : public Command
{
    Q_OBJECT
public:
    explicit AddressBookCommand(QObject *parent = nullptr);
    ~AddressBookCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;
};

#endif // GIKWIMICTL_ADDRESSBOOKCOMMAND_H
