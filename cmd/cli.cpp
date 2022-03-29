/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "cli.h"
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <cstdio>

CLI::CLI(bool quiet)
    : m_quiet{quiet}
{

}

void CLI::printError(const QString &error) const
{
    if (!m_quiet) {
        std::fprintf(stderr, "\x1b[33m%s\x1b[0m\n", error.toUtf8().constData());
    }
}

void CLI::printError(const QStringList &errors) const
{
    if (!m_quiet && !errors.empty()) {
        for (const QString &error : errors) {
            printError(error);
        }
    }
}

void CLI::printStatus(const QString &status) const
{
    if (!m_quiet) {
        std::printf("%-100s", status.toUtf8().constData());
    }
}

void CLI::printDone() const
{
    //% "Done"
    printDone(qtTrId("gikwimictl-msg-done"));
}

void CLI::printDone(const QString &done) const
{
    if (!m_quiet) {
        std::printf("\x1b[32m%s\x1b[0m\n", qUtf8Printable(done));
    }
}

void CLI::printFailed() const
{
    //% "Failed"
    printFailed(qtTrId("gikwimictl-msg-failed"));
}

void CLI::printFailed(const QString &failed) const
{
    if (!m_quiet) {
        printf("\x1b[31m%s\x1b[0m\n", qUtf8Printable(failed));
    }
}
