/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_DATABASECOMMAND_H
#define GIKWIMICTL_DATABASECOMMAND_H

#include "../command.h"
#include <QObject>
#include <memory>

namespace Firfuorida {
class Migrator;
}

class DatabaseCommand : public Command
{
    Q_OBJECT
public:
    explicit DatabaseCommand(QObject *parent = nullptr);

    ~DatabaseCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;

protected:
    QString dbConName() const;
    void initMigrations();
    std::unique_ptr<Firfuorida::Migrator> m_migrator;
};

#endif // GIKWIMICTL_DATABASECOMMAND_H
