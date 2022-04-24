/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_MENUITEM_P_H
#define GIKWIMI_MENUITEM_P_H

#include "menuitem.h"
#include <QSharedData>

class MenuItemData : public QSharedData
{
public:
    MenuItemData() = default;

    ~MenuItemData() = default;

    std::vector<MenuItem> children;
    QString icon;
    QString name;
    QString title;
    QUrl url;
    bool isActive = false;
    bool isExpanded = false;
};

#endif // GIKWIMI_MENUITEM_P_H
