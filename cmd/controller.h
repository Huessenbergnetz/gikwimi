/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_CONTROLLER_H
#define GIKWIMICTL_CONTROLLER_H

#include <QObject>
#include <QCommandLineOption>

class QCommandLineParser;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    ~Controller() override;

    void init();

    int exec(QCommandLineParser *parser);

    QList<QCommandLineOption> globalOptions() const;

private:
    void showHelp() const;
    QList<QCommandLineOption> m_globalOptions;
};

#endif // GIKWIMICTL_CONTROLLER_H
