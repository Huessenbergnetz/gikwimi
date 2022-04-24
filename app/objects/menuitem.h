/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_MENUITEM_H
#define GIKWIMI_MENUITEM_H

#include <Cutelyst/ParamsMultiMap>

#include <QObject>
#include <QSharedDataPointer>
#include <QUrl>

#include <vector>

using namespace Cutelyst;

namespace Cutelyst {
class Context;
}

class MenuItemData;
class MenuItem
{
    Q_GADGET
    Q_PROPERTY(std::vector<MenuItem> children READ children CONSTANT)
    Q_PROPERTY(QString icon READ icon CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(bool isActive READ isActive CONSTANT)
    Q_PROPERTY(bool hasChildren READ hasChildren CONSTANT)
    Q_PROPERTY(bool isExpanded READ isExpanded CONSTANT)
public:
    MenuItem();

    MenuItem(Cutelyst::Context *c, const QString &name, const QString &title, const QString &icon, const QString &action, const QString &ns = QString(), const QStringList &captures = QStringList(), const QStringList &args = QStringList(), const ParamsMultiMap &queryValues = ParamsMultiMap());

    MenuItem(const MenuItem &other);

    MenuItem(MenuItem &&other) noexcept;

    MenuItem &operator=(const MenuItem &other);

    MenuItem &operator=(MenuItem &&other) noexcept;

    ~MenuItem();

    void swap(MenuItem &other) noexcept;

    std::vector<MenuItem> children() const;

    QString icon() const;

    QString name() const;

    QString title() const;

    QUrl url() const;

    bool isActive() const;

    bool hasChildren() const;

    bool isExpanded() const;

protected:
    QSharedDataPointer<MenuItemData> d;
};

Q_DECLARE_METATYPE(MenuItem)
Q_DECLARE_TYPEINFO(MenuItem, Q_MOVABLE_TYPE);


#endif // GIKWIMI_MENUITEM_H
