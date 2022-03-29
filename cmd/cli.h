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
    void printError(const QString &error) const;

    void printError(const QStringList &errors) const;

    void printStatus(const QString &status) const;

    void printDone() const;

    void printDone(const QString &done) const;

    void printFailed() const;

    void printFailed(const QString &failed) const;

private:
    bool m_quiet;
};

#endif // GIKWIMICTL_CLI_H
