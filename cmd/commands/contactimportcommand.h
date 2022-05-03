/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_CONTACTIMPORTCOMMAND_H
#define GIKWIMICTL_CONTACTIMPORTCOMMAND_H

#include "../command.h"
#include <QObject>

class ContactImportCommand : public Command
{
    Q_OBJECT
public:
    enum class Strategy : qint8 {
        Invalid,
        Replace,
        KeepOld,
        KeepNew,
        Add
    };
    Q_ENUM(Strategy)

    explicit ContactImportCommand(QObject *parent = nullptr);
    ~ContactImportCommand() override;

    void init() override;

    int exec(QCommandLineParser *parser) override;

    QString summary() const override;

    QString description() const override;

private:
    Strategy strategyStringToEnum(const QString &str) const;
    QString strategyEnumToString(Strategy strategy) const;
    QStringList supportedStrategies() const;
};

#endif // GIKWIMICTL_CONTACTIMPORTCOMMAND_H
