/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_GIKWIMI_H
#define GIKWIMI_GIKWIMI_H

#include <Cutelyst/Application>

using namespace Cutelyst;

class Gikwimi : public Application
{
    Q_OBJECT
    CUTELYST_APPLICATION(IID "Gikwimi")
public:
    /*!
     * \brief Constructs a new application instance with the given \a parent.
     */
    Q_INVOKABLE explicit Gikwimi(QObject *parent = nullptr);

    /*!
     * \brief Destroys the application instance.
     */
    ~Gikwimi() override;

    /*!
     * \brief Initializes the application and returns \c true on success.
     */
    bool init() override;

    /*!
     * \brief This will be called after the engine forked and will setup the database connection.
     *
     * Returns always \c true.
     */
    bool postFork() override;

private:
    bool initDb();
    static bool isInitialized;
    static bool messageHandlerInstalled;
};

#endif // GIKWIMI_GIKWIMI_H
