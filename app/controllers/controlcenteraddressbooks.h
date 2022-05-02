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
    void index(Context *c);

    C_ATTR(base, :Chained("/") :PathPart("controlcenter/addressbooks") :CaptureArgs(1))
    void base(Context *c, const QString &id);

    C_ATTR(edit, :Chained("base") :PathPart("edit") :Args(0))
    void edit(Context *c);

    C_ATTR(addresses, :Chained("base") :PathPart("addresses") :Args(0))
    void addresses(Context *c);

    C_ATTR(addAddress, :Chained("base") :PathPart("addresses/add") :Args(0))
    void addAddress(Context *c);

    C_ATTR(editAddress, :Chained("base") :PathPart("addresses/edit") :Args(1))
    void editAddress(Context *c, const QString &id);

    C_ATTR(add, :Path("add") :Args(0))
    void add(Context *c);

private:
    void buildMenu(Context *c);
};

#endif // GIKWIMI_CONTROLCENTERADDRESSBOOK_H
