/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "optionitem.h"

class OptionItemData : public QSharedData
{
public:
    QString value;
    QString name;
    bool selected = false;
};

OptionItem::OptionItem() : d(new OptionItemData)
{

}

OptionItem::OptionItem(const QString &name, const QString &value, bool selected)
    : d(new OptionItemData)
{
    d->name = name;
    d->value = value;
    d->selected = selected;
}

OptionItem::OptionItem(const QString &name, int value, bool selected)
    : d(new OptionItemData)
{
    d->name = name;
    d->value = QString::number(value);
    d->selected = selected;
}

OptionItem::OptionItem(const OptionItem &other) = default;

OptionItem::OptionItem(OptionItem &&other) noexcept = default;

OptionItem &OptionItem::operator=(const OptionItem &rhs) = default;

OptionItem &OptionItem::operator=(OptionItem &&other) noexcept = default;

OptionItem::~OptionItem() = default;

QString OptionItem::name() const
{
    return d->name;
}

QString OptionItem::value() const
{
    return d->value;
}

bool OptionItem::selected() const
{
    return d->selected;
}

#include "moc_optionitem.cpp"
