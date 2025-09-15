/*
 * SPDX-FileCopyrightText: (C) 2022, 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_USERAUTHSTORESQL_H
#define GIKWIMI_USERAUTHSTORESQL_H

#include <Cutelyst/Plugins/Authentication/authenticationstore.h>

using namespace Cutelyst;

class UserAuthStoreSql : public AuthenticationStore
{
    Q_DISABLE_COPY(UserAuthStoreSql)
public:
    UserAuthStoreSql();
    ~UserAuthStoreSql() override;

    AuthenticationUser findUser(Context *c, const ParamsMultiMap &userinfo) override;
};

#endif // GIKWIMI_USERAUTHSTORESQL_H
