/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "menuitem_p.h"

#include <Cutelyst/Context>
#include <Cutelyst/Action>
#include <Cutelyst/Controller>

MenuItem::MenuItem()
    : d(new MenuItemData)
{

}

MenuItem::MenuItem(Cutelyst::Context *c, const QString &name, const QString &title, const QString &icon, const QString &action, const QString &ns, const QStringList &captures, const QStringList &args, const ParamsMultiMap &queryValues)
    : d(new MenuItemData)
{
    d->name = name;
    d->title = title;
    d->icon = icon;
    auto _action = c->getAction(action, ns);
    d->url = c->uriFor(_action, captures, args, queryValues);
    d->isActive = _action == c->action();
}

MenuItem::MenuItem(const MenuItem &other) = default;

MenuItem::MenuItem(MenuItem &&other) noexcept = default;

MenuItem &MenuItem::operator=(const MenuItem &other) = default;

MenuItem &MenuItem::operator=(MenuItem &&other) noexcept = default;

MenuItem::~MenuItem() = default;

void MenuItem::swap(MenuItem &other) noexcept
{
    std::swap(d, other.d);
}

std::vector<MenuItem> MenuItem::children() const
{
    return d->children;
}

QString MenuItem::icon() const
{
    return d->icon;
}

QString MenuItem::name() const
{
    return d->name;
}

QString MenuItem::title() const
{
    return d->title;
}

QUrl MenuItem::url() const
{
    return d->url;
}

bool MenuItem::isActive() const
{
    return d->isActive;
}

bool MenuItem::hasChildren() const
{
    return !d->children.empty();
}

bool MenuItem::isExpanded() const
{
    return d->isExpanded;
}
