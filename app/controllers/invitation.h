/*
 * SPDX-FileCopyrightText: (C) 2022, 2025 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GIKWIMI_INVITATION_H
#define GIKWIMI_INVITATION_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Guest;
class Event;

class Invitation : public Controller
{
    Q_OBJECT
    Q_DISABLE_COPY(Invitation)
    C_NAMESPACE("i")
public:
    explicit Invitation(QObject *parent = nullptr);
    ~Invitation() override;

    C_ATTR(index, :Path :Args(1))
    void index(Context *c, const QString &uid);

private:
    void sendNotificationEmails(Context *c, const Guest &guest, const Event &event);
};

#endif // GIKWIMI_INVITATION_H
