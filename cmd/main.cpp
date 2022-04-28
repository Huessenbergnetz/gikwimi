/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QCommandLineParser>

#include "controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Huessenbergnetz"));
    app.setOrganizationDomain(QStringLiteral("huessenbergnetz.de"));
    app.setApplicationName(QStringLiteral("gikwimictl"));
    app.setApplicationVersion(QStringLiteral(GIKWIMI_VERSION));

    const QLocale locale;
    {
        auto trans = new QTranslator(&app);
        if (Q_LIKELY(trans->load(locale, QStringLiteral("gikwimictl"), QStringLiteral("_"), QStringLiteral(GIKWIMI_TRANSLATIONSDIR)))) {
            if (Q_UNLIKELY(!QCoreApplication::installTranslator(trans))) {
                qWarning("Can not install translator for locale %s", qUtf8Printable(locale.name()));
            }
        } else {
            qWarning("Can not load translations for locale %s from %s", qUtf8Printable(locale.name()), GIKWIMI_TRANSLATIONSDIR);
        }
    }

    QCommandLineParser parser;

    auto c = new Controller(&app);
    c->init();
    return c->exec(&parser);

    return 0;
}
