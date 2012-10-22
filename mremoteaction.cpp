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

#include "mremoteaction.h"
#include "mremoteaction_p.h"

#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QBuffer>
#include <QStringList>

MRemoteActionPrivate::MRemoteActionPrivate()
{
}

MRemoteActionPrivate::~MRemoteActionPrivate()
{
}

MRemoteAction::MRemoteAction(const QString &serviceName, const QString &objectPath, const QString &interface, const QString &methodName, const QList<QVariant> &arguments, QObject *parent) :
    QObject(parent),
    d_ptr(new MRemoteActionPrivate)
{
    Q_D(MRemoteAction);

    d->serviceName = serviceName;
    d->objectPath = objectPath;
    d->interface = interface;
    d->methodName = methodName;
    d->arguments = arguments;
}

MRemoteAction::MRemoteAction(const QString &string, QObject *parent) :
    QObject(parent),
    d_ptr(new MRemoteActionPrivate)
{
    fromString(string);
}

MRemoteAction::~MRemoteAction()
{
    delete d_ptr;
}

QString MRemoteAction::toString() const
{
    Q_D(const MRemoteAction);

    QString s;
    if (!d->serviceName.isEmpty() && !d->objectPath.isEmpty() && !d->interface.isEmpty() && !d->methodName.isEmpty()) {
        s.append(d->serviceName).append(' ');
        s.append(d->objectPath).append(' ');
        s.append(d->interface).append(' ');
        s.append(d->methodName);

        foreach(const QVariant & arg, d->arguments) {
            // Serialize the QVariant into a QBuffer
            QBuffer buffer;
            buffer.open(QIODevice::ReadWrite);
            QDataStream stream(&buffer);
            stream << arg;
            buffer.close();

            // Encode the contents of the QBuffer in Base64
            s.append(' ');
            s.append(buffer.buffer().toBase64().data());
        }
    }

    return s;
}

void MRemoteAction::fromString(const QString &string)
{
    Q_D(MRemoteAction);

    QStringList l = string.split(' ');

    if (l.count() > 3) {
        d->serviceName = l.at(0);
        d->objectPath = l.at(1);
        d->interface = l.at(2);
        d->methodName = l.at(3);
    }

    const int count = l.count();
    for (int i = 4; i < count; ++i) {
        QByteArray byteArray = QByteArray::fromBase64(l.at(i).toAscii());
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::ReadOnly);
        QDataStream stream(&buffer);
        QVariant arg;
        stream >> arg;
        buffer.close();

        d->arguments.append(arg);
    }
}

MRemoteAction::MRemoteAction(const MRemoteAction &action) :
    QObject(action.parent()),
    d_ptr(new MRemoteActionPrivate)
{
    fromString(action.toString());
}

void MRemoteAction::trigger()
{
    Q_D(MRemoteAction);

    QDBusInterface interface(d->serviceName, d->objectPath, d->interface.toAscii());
    if (interface.isValid()) {
        interface.asyncCallWithArgumentList(d->methodName, d->arguments);
    }
}
