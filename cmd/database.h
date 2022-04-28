/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_DATABASE_H
#define GIKWIMICTL_DATABASE_H

#include "configuration.h"

#include <QObject>

class Database : public Configuration
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    ~Database();

protected:
    int openDb(const QString &connectionName);
};

#endif // GIKWIMICTL_DATABASE_H
