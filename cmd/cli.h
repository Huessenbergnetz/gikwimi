/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_CLI_H
#define GIKWIMICTL_CLI_H

#include <QString>
#include <QStringList>
#include <limits>
class QSqlError;
class QSqlQuery;
class QSqlDatabase;

class CLI
{
public:
    explicit CLI(bool quiet = false);

protected:
    void printError(const QString &error) const;

    void printError(const QStringList &errors) const;

    int error(const QString &error, int code) const;

    int inputError(const QString &error) const;

    int configError(const QString &error) const;

    int fileError(const QString &error) const;

    int dbError(const QString &error) const;

    int dbError(const QSqlError &error) const;

    int dbError(const QSqlQuery &query) const;

    int dbError(const QSqlDatabase &db) const;

    void printStatus(const QString &status) const;

    void printDone() const;

    void printDone(const QString &done) const;

    void printFailed() const;

    void printFailed(const QString &failed) const;

    void printMessage(const QString &message) const;

    void printSuccess(const QString &message) const;

    void printDesc(const QString &description) const;

    void printDesc(const QStringList &description) const;

    QString readString(const QString &name, const QString &defaultValue = QString(), const QStringList &acceptableInput = QStringList()) const;

    int readInt(const QString &name, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max()) const;

    int readIntWithDef(const QString &name, int defaultValue, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max()) const;

    bool readBool(const QString &name, bool deaultValue = false) const;

    int readPort(const QString &name, int defaultValue) const;

private:
    bool m_quiet = false;

    void printInputline(const QString &name, const QString &defaultValue = QString()) const;

    QString readInputline() const;
};

#endif // GIKWIMICTL_CLI_H
