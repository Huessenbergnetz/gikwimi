/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_UTILS_H
#define GIKWIMI_UTILS_H

namespace Cutelyst {
class Context;
}

class Utils
{
public:
    static bool isAdmin(Cutelyst::Context *c);
};

#endif // GIKWIMI_UTILS_H
