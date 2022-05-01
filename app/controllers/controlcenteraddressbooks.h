/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_CONTROLCENTERADDRESSBOOK_H
#define GIKWIMI_CONTROLCENTERADDRESSBOOK_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class ControlCenterAddressBooks : public Controller
{
    Q_OBJECT
    C_NAMESPACE("controlcenter/addressbooks")
public:
    explicit ControlCenterAddressBooks(QObject *parent = nullptr);
    ~ControlCenterAddressBooks() override;

    C_ATTR(index, :Path :Args(0))
    void index(Context *);

    C_ATTR(add, :Path("add") :Args(0))
    void add(Context *c);

    C_ATTR(edit, :Path("edit") :Args(1))
    void edit(Context *c, const QString &id);

private:
    void buildMenu(Context *c);
};

#endif // GIKWIMI_CONTROLCENTERADDRESSBOOK_H
