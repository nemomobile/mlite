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

#include "mnotificationgroup.h"
#include "mnotificationgroup_p.h"

MNotificationGroupPrivate::MNotificationGroupPrivate() : MNotificationPrivate()
{
}

MNotificationGroup::MNotificationGroup() :
    MNotification(*new MNotificationGroupPrivate)
{
}

MNotificationGroup::MNotificationGroup(const QString &eventType, const QString &summary, const QString &body) :
    MNotification(*new MNotificationGroupPrivate)
{
    Q_D(MNotificationGroup);
    d->eventType = eventType;
    d->summary = summary;
    d->body = body;
}

MNotificationGroup::MNotificationGroup(const MNotificationGroup &group) :
    MNotification(*new MNotificationGroupPrivate)
{
    *this = group;
}

MNotificationGroup::~MNotificationGroup()
{
}

MNotificationGroup::MNotificationGroup(uint id) : MNotification(id)
{
}

QVariantHash MNotificationGroupPrivate::hints() const
{
    QVariantHash hints;
    hints.insert("category", eventType);
    hints.insert("x-nemo-item-count", count);
    hints.insert("x-nemo-timestamp", userSetTimestamp);
    return hints;
}

bool MNotificationGroup::publish()
{
    Q_D(MNotificationGroup);
    d->groupId = d->id;
    return MNotification::publish();
}

uint MNotificationGroup::notificationCount()
{
    return 0;
}

QList<MNotificationGroup *> MNotificationGroup::notificationGroups()
{
    return QList<MNotificationGroup *>();
}

void MNotificationGroup::setTimestamp(const QDateTime &)
{
}
