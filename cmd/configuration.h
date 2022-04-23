/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMICTL_CONFIGURATION_H
#define GIKWIMICTL_CONFIGURATION_H

#include "cli.h"

#include <QString>
#include <QVariantMap>

class Configuration : public CLI
{
public:
    explicit Configuration(const QString &iniPath, bool quiet = false);

    QVariant value(const QString &group, const QString &key, const QVariant &defaultValue = QVariant()) const;

protected:
    int loadConfig();

private:
    QString m_iniPath;
    QVariantMap m_config;
    bool m_loaded = false;
};

#endif // GIKWIMICTL_CONFIGURATION_H
