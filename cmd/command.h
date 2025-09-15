/*
 * SPDX-FileCopyrightText: (C) 2022, 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "database.h"
#include <QObject>
#include <QCommandLineOption>

class QCommandLineParser;
class QTextStream;

class Command : public Database
{
    Q_OBJECT
public:
    explicit Command(QObject *parent = nullptr);
    ~Command() override;

    virtual void init() = 0;

    virtual int exec(QCommandLineParser *parser) = 0;

    virtual QString summary() const = 0;
    virtual QString description() const = 0;
    void showHelp() const;

protected:
    void showGlobalOptions(QTextStream *out) const;
    void showUsage(QTextStream *out) const;
    void showSubCommands(QTextStream *out) const;
    void showOptions(QTextStream *out) const;
    void setGlobalOptions(QCommandLineParser *parser);
    bool checkShowHelp(QCommandLineParser *parser);
    int runSubCommand(const QString &command, QCommandLineParser *parser);

    QList<QCommandLineOption> m_cliOptions;

private:
    qsizetype getLongestOptionName(const QList<QCommandLineOption> &options) const;
    void showOptions(QTextStream *out, const QList<QCommandLineOption> &options) const;
};

#endif // COMMAND_H
