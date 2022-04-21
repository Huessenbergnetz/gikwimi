/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "configuration.h"
#include <QSettings>
#include <QFileInfo>

Configuration::Configuration(const QString &iniPath, bool quiet)
    : CLI{quiet}, m_iniPath{iniPath}
{

}

int Configuration::loadConfig()
{
    //% "Reading configuration file"
    printStatus(qtTrId("gikctl-status-reading-config"));

    QSettings conf(m_iniPath, QSettings::IniFormat);

    switch(conf.status()) {
    case QSettings::NoError:
        break;
    case QSettings::AccessError:
    {
        printFailed();
        //: Error message, %1 will be replaced by the file path
        //% "Can not read configuration file at %1"
        return fileError(qtTrId("gikctl-error-config-file-not-readable").arg(m_iniPath));
    }
    case QSettings::FormatError:
    {
        printFailed();
        //: Error message, %1 will be replaced by the file path
        //% "Failed to parse configuration file at %1"
        return configError(qtTrId("gikctl-error-config-file-malformed").arg(m_iniPath));
    }
    }

    const QStringList confGroups = conf.childGroups();

    for (const QString &group : confGroups) {
        QVariantMap map = m_config.value(group, QVariantMap()).toMap();
        conf.beginGroup(group);
        const QStringList keys = conf.childKeys();
        for (const QString &key : keys) {
            map.insert(key, conf.value(key));
        }
        conf.endGroup();
        m_config.insert(group, map);
    }

    printDone();

    m_loaded = true;

    return 0;
}

QVariant Configuration::value(const QString &group, const QString &key, const QVariant &defaultValue) const
{
    if (Q_UNLIKELY(!m_loaded)) {
        return QVariant();
    }

    const QVariantMap map = m_config.value(group).toMap();

    return map.value(key, defaultValue);
}
