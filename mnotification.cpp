/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QDBusConnection>
#include <QCoreApplication>
#include <QFileInfo>
#include "mnotification.h"
#include "mnotification_p.h"
#include "mnotificationgroup.h"
#include "mnotificationmanagerproxy.h"
#include "mremoteaction.h"

const QString MNotification::DeviceEvent = "device";
const QString MNotification::DeviceAddedEvent = "device.added";
const QString MNotification::DeviceErrorEvent = "device.error";
const QString MNotification::DeviceRemovedEvent = "device.removed";
const QString MNotification::EmailEvent = "email";
const QString MNotification::EmailArrivedEvent = "email.arrived";
const QString MNotification::EmailBouncedEvent = "email.bounced";
const QString MNotification::ImEvent = "im";
const QString MNotification::ImErrorEvent = "im.error";
const QString MNotification::ImReceivedEvent = "im.received";
const QString MNotification::NetworkEvent = "network";
const QString MNotification::NetworkConnectedEvent = "network.connected";
const QString MNotification::NetworkDisconnectedEvent = "network.disconnected";
const QString MNotification::NetworkErrorEvent = "network.error";
const QString MNotification::PresenceEvent = "presence";
const QString MNotification::PresenceOfflineEvent = "presence.offline";
const QString MNotification::PresenceOnlineEvent = "presence.online";
const QString MNotification::TransferEvent = "transfer";
const QString MNotification::TransferCompleteEvent = "transfer.complete";
const QString MNotification::TransferErrorEvent = "transfer.error";

QPointer<MNotificationManagerProxy> MNotificationPrivate::notificationManager;
QString MNotificationPrivate::appName;

MNotificationPrivate::MNotificationPrivate() :
    id(0),
    groupId(0),
    count(0),
    userSetTimestamp(0),
    publishedTimestamp(0)
{
    if (notificationManager.isNull()) {
        notificationManager = new MNotificationManagerProxy("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus());
        appName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    }

    connect(notificationManager.data(), SIGNAL(ActionInvoked(uint,QString)), this, SLOT(invokeAction(uint,QString)));
}

MNotificationPrivate::~MNotificationPrivate()
{
}

QVariantHash MNotificationPrivate::hints() const
{
    QVariantHash hints;
    hints.insert("category", eventType);
    hints.insert("x-nemo-item-count", count);
    hints.insert("x-nemo-timestamp", userSetTimestamp);
    hints.insert("x-nemo-preview-summary", summary);
    hints.insert("x-nemo-preview-body", body);
    return hints;
}

void MNotificationPrivate::invokeAction(uint id, const QString &actionKey)
{
    if (id == this->id && actionKey == "clicked") {
        MRemoteAction(action).trigger();
    }
}

MNotification::MNotification(MNotificationPrivate &dd) :
    d_ptr(&dd)
{
}

MNotification::MNotification() :
    d_ptr(new MNotificationPrivate)
{
}

MNotification::MNotification(const QString &eventType, const QString &summary, const QString &body) :
    d_ptr(new MNotificationPrivate)
{
    Q_D(MNotification);
    d->eventType = eventType;
    d->summary = summary;
    d->body = body;
}

MNotification::MNotification(const MNotification &notification) :
    QObject(), d_ptr(new MNotificationPrivate)
{
    *this = notification;
}

MNotification::MNotification(uint id) :
    d_ptr(new MNotificationPrivate)
{
    Q_D(MNotification);
    d->id = id;
}

MNotification::~MNotification()
{
    delete d_ptr;
}

uint MNotification::id() const
{
    Q_D(const MNotification);
    return d->id;
}

void MNotification::setGroup(const MNotificationGroup &group)
{
    Q_D(MNotification);
    d->groupId = group.id();
}

void MNotification::setEventType(const QString &eventType)
{
    Q_D(MNotification);
    d->eventType = eventType;
}

QString MNotification::eventType() const
{
    Q_D(const MNotification);
    return d->eventType;
}

void MNotification::setSummary(const QString &summary)
{
    Q_D(MNotification);
    d->summary = summary;
}

QString MNotification::summary() const
{
    Q_D(const MNotification);
    return d->summary;
}

void MNotification::setBody(const QString &body)
{
    Q_D(MNotification);
    d->body = body;
}

QString MNotification::body() const
{
    Q_D(const MNotification);
    return d->body;
}

void MNotification::setImage(const QString &image)
{
    Q_D(MNotification);
    d->image = image;
}

QString MNotification::image() const
{
    Q_D(const MNotification);
    return d->image;
}

void MNotification::setAction(const MRemoteAction &action)
{
    Q_D(MNotification);
    d->action = action.toString();
}

void MNotification::setCount(uint count)
{
    Q_D(MNotification);
    d->count = count;
}

uint MNotification::count() const
{
    Q_D(const MNotification);
    return d->count;
}

void MNotification::setIdentifier(const QString &identifier)
{
    Q_D(MNotification);
    d->identifier = identifier;
}

QString MNotification::identifier() const
{
    Q_D(const MNotification);
    return d->identifier;
}

void MNotification::setTimestamp(const QDateTime &timestamp)
{
    Q_D(MNotification);
    d->userSetTimestamp = timestamp.isValid() ? timestamp.toTime_t() : 0;
}

const QDateTime MNotification::timestamp() const
{
    Q_D(const MNotification);
    return d->publishedTimestamp != 0 ? QDateTime::fromTime_t(d->publishedTimestamp) : QDateTime();
}

bool MNotification::publish()
{
    Q_D(MNotification);

    if (d->userSetTimestamp == 0) {
        d->userSetTimestamp = QDateTime::currentDateTimeUtc().toTime_t();
    }

    if (d->groupId == 0) {
        // Standalone notification: preview banner and notification use the same summary and body
        d->id = d->notificationManager->Notify(d->appName, d->id, d->image, d->summary, d->body, QStringList() << "clicked", d->hints(), -1);
    } else {
        // Notification in a group: show summary and body in preview banner only
        d->id = d->notificationManager->Notify(d->appName, d->id, d->image, QString(), QString(), QStringList() << "clicked", d->hints(), -1);
    }

    if (d->id != 0) {
        d->publishedTimestamp = d->userSetTimestamp;
    }

    d->userSetTimestamp = 0;

    return d->id != 0;
}

bool MNotification::remove()
{
    bool success = false;

    if (isPublished()) {
        Q_D(MNotification);
        d->notificationManager->CloseNotification(d->id);
        d->id = 0;
        success = true;
    }

    return success;
}

bool MNotification::isPublished() const
{
    Q_D(const MNotification);
    return d->id != 0;
}

QList<MNotification *> MNotification::notifications()
{
    return QList<MNotification *>();
}

MNotification &MNotification::operator=(const MNotification &notification)
{
    Q_D(MNotification);
    const MNotificationPrivate *dn = notification.d_func();
    d->id = dn->id;
    d->groupId = dn->groupId;
    d->eventType = dn->eventType;
    d->summary = dn->summary;
    d->body = dn->body;
    d->image = dn->image;
    d->action = dn->action;
    d->count = dn->count;
    d->identifier = dn->identifier;
    d->userSetTimestamp = dn->userSetTimestamp;
    d->publishedTimestamp = dn->publishedTimestamp;
    return *this;
}
