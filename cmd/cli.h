/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_CLI_H
#define GIKWIMICTL_CLI_H

class QString;
class QStringList;

class CLI
{
public:
    explicit CLI(bool quiet = false);

protected:
    void printError(const QString &error);

    void printError(const QStringList &errors);

    void printStatus(const QString &status);

    void printDone();

    void printDone(const QString &done);

    void printFailed();

    void printFailed(const QString &failed);

private:
    bool m_quiet;
};

#endif // GIKWIMICTL_CLI_H
