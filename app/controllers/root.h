/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_ROOT_H
#define GIKWIMI_ROOT_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Root : public Controller
{
    Q_OBJECT
    C_NAMESPACE("")
public:
    explicit Root(QObject *parent = nullptr);
    ~Root() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *c);

    C_ATTR(defaultPage, :Path)
    void defaultPage(Context *c);

    C_ATTR(error, :Local :Private :AutoArgs :ActionClass("RenderView"))
    void error(Context *c);

private:
    C_ATTR(End, :ActionClass("RenderView"))
    void End(Context *c) { Q_UNUSED(c); }

    C_ATTR(Auto, :Private)
    bool Auto(Context *c);
};

#endif //GIKWIMI_ROOT_H
