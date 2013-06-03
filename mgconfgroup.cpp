/***************************************************************************
** Copyright (C) 2013 Jolla Mobile <andrew.den.exter@jollamobile.com>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "mgconfgroup.h"
#include "mgconfitem_p.h"

#include <QDebug>
#include <QMetaProperty>

class MGConfGroupPrivate
{
public:
    MGConfGroupPrivate()
        : group(0)
        , scope(0)
        , client(0)
        , notifyId(0)
        , readPropertyIndex(-1)
        , propertyOffset(-1)
    {
    }

    void resolveProperties(const QByteArray &scopePath);

    void cancelNotifications();
    void readValue(const QMetaProperty &property, GConfValue *value);

    static void notify(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data);

    QByteArray absolutePath;
    QString path;
    QList<QObject *> data;
    QList<MGConfGroup *> children;
    MGConfGroup *group;
    MGConfGroup *scope;

    GConfClient *client;
    guint notifyId;

    int readPropertyIndex;
    int propertyOffset;
};

MGConfGroup::MGConfGroup(QObject *parent, MetaObjectResolution resolution)
    : QObject(parent)
    , priv(new MGConfGroupPrivate)
{
    priv->group = this;
    priv->client = MGConfItemPrivate::client();
    if (resolution == ImmediateResolve)
        resolveMetaObject(staticMetaObject.propertyCount());
}

MGConfGroup::~MGConfGroup()
{
    if (priv->client) {
        if (!priv->scope && !priv->absolutePath.isEmpty()) {
            priv->absolutePath.chop(1);
            gconf_client_remove_dir(priv->client, priv->absolutePath.constData(), 0);
        }

        priv->cancelNotifications();

        g_object_unref(priv->client);
    }
    delete priv;
}

void MGConfGroup::resolveMetaObject(int propertyOffset)
{
    if (priv->propertyOffset >= 0)
        return;

    const int propertyChangedIndex = staticMetaObject.indexOfMethod("propertyChanged()");
    Q_ASSERT(propertyChangedIndex != -1);

    priv->propertyOffset = propertyOffset;

    const QMetaObject * const metaObject = this->metaObject();
    if (metaObject == &staticMetaObject)
        return;

    for (int i = propertyOffset; i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);

        if (property.hasNotifySignal()) {
            QMetaObject::connect(this, property.notifySignalIndex(), this, propertyChangedIndex);
        }
    }

    if (priv->path.startsWith(QLatin1Char('/'))) {
        GError *error = 0;
        gconf_client_add_dir(
                    priv->client,
                    priv->path.toUtf8().constData(),
                    GCONF_CLIENT_PRELOAD_RECURSIVE,
                    &error);
        if (error) {
            qWarning() << "Failed to register listener for path " << priv->path;
            qWarning() << error->message;
            g_error_free(error);
        }
        priv->resolveProperties(QByteArray());
    } else if (priv->scope && !priv->path.isEmpty() && !priv->scope->priv->absolutePath.isEmpty()) {
        priv->resolveProperties(priv->scope->priv->absolutePath);
    }
}

QString MGConfGroup::path() const
{
    return priv->path;
}

void MGConfGroup::setPath(const QString &path)
{
    if (priv->path != path) {
        if (priv->client && !priv->absolutePath.isEmpty()) {
            priv->cancelNotifications();

            if (priv->path.startsWith(QLatin1Char('/'))) {
                priv->absolutePath.chop(1);
                gconf_client_remove_dir(priv->client, priv->absolutePath.constData(), 0);
                priv->absolutePath = QByteArray();
            }
        }

        priv->path = path;
        emit pathChanged();

        if (priv->path.isEmpty() || priv->propertyOffset < 0) {
            // Don't do any of the following things.
        } else if (priv->path.startsWith(QLatin1Char('/'))) {
            GError *error = 0;
            gconf_client_add_dir(
                        priv->client,
                        priv->path.toUtf8().constData(),
                        GCONF_CLIENT_PRELOAD_RECURSIVE,
                        &error);
            if (error) {
                qWarning() << "Failed to register listener for path " << priv->path;
                qWarning() << error->message;
                g_error_free(error);
            }
            priv->resolveProperties(QByteArray());
        } else if (priv->scope && !priv->scope->priv->absolutePath.isEmpty()) {
            priv->resolveProperties(priv->scope->priv->absolutePath);
        }
    }
}

MGConfGroup *MGConfGroup::scope() const
{
    return priv->scope;
}

void MGConfGroup::setScope(MGConfGroup *scope)
{
    if (scope != priv->scope) {
        if (priv->scope) {
            priv->scope->priv->children.removeAll(this);
            if (priv->client
                    && !priv->absolutePath.isEmpty()
                    && !priv->path.startsWith(QLatin1Char('/'))) {
                priv->cancelNotifications();
            }
        }

        priv->scope = scope;

        if (priv->scope) {
            priv->scope->priv->children.append(this);
            if (priv->client
                    && !priv->path.startsWith(QLatin1Char('/'))
                    && !priv->scope->priv->absolutePath.isEmpty()) {
                priv->resolveProperties(priv->scope->priv->absolutePath);
            }
        }

        emit scopeChanged();
    }
}

void MGConfGroup::clear()
{
    if (!priv->absolutePath.isEmpty()) {
        QByteArray key = priv->absolutePath;
        key.chop(1);
        GError *error = 0;
        gconf_client_recursive_unset(priv->client, key.constData(), GConfUnsetFlags(0), &error);
        if (error) {
            qWarning() << "Failed to unset values for " << priv->absolutePath;
            qWarning() << error->message;
            g_error_free(error);
        }
    }
}

void MGConfGroup::propertyChanged()
{
    if (priv->absolutePath.isEmpty())
        return;

    const int notifyIndex = senderSignalIndex();
    const QMetaObject * const metaObject = this->metaObject();

    for (int i = priv->propertyOffset; i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (i != priv->readPropertyIndex && property.notifySignalIndex() == notifyIndex) {
            const QByteArray key = priv->absolutePath + property.name();
            const QVariant variant = property.read(this);

            GError *error = 0;
            if (GConfValue *value = MGConfItemPrivate::fromVariant(variant)) {
                gconf_client_set(priv->client, key.constData(), value, &error);
                gconf_value_free(value);
            } else if (variant.type() == QVariant::Invalid
                       || variant.userType() == qMetaTypeId<QVariantList>()) {
                gconf_client_unset(priv->client, key.constData(), &error);
            }

            if (error) {
                qWarning() << "Failed to write value for " << key << variant;
                qWarning() << error->message;
                g_error_free(error);
            }
        }
    }
}

void MGConfGroupPrivate::resolveProperties(const QByteArray &scopePath)
{
    GError *error = 0;

    absolutePath = scopePath + path.toUtf8();
    notifyId = gconf_client_notify_add(client, absolutePath.constData(), notify, this, 0, &error);
    if (error) {
        qWarning() << "Failed to register notifications for " << absolutePath;
        qWarning() << error->message;
        g_error_free(error);
        error = 0;
    }
    absolutePath += '/';

    const QMetaObject * const metaObject = group->metaObject();
    for (int i = propertyOffset; i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        const QByteArray key = absolutePath + property.name();

        GConfValue *value = gconf_client_get(client, key.constData(), &error);
        if (error) {
            qWarning() << "Failed to get value for " << key;
            qWarning() << error->message;
            g_error_free(error);
            error = 0;
        } else if (value) {
            readPropertyIndex = i;
            readValue(property, value);
            readPropertyIndex = -1;
            gconf_value_free(value);
        }
    }

    for (int i = 0; i < children.count(); ++i) {
        children.at(i)->priv->resolveProperties(absolutePath);
    }
}

void MGConfGroupPrivate::readValue(const QMetaProperty &property, GConfValue *value)
{
    QVariant variant = MGConfItemPrivate::toVariant(value);
    if (variant.isValid()) {
        property.write(group, variant);
    }
}

void MGConfGroupPrivate::cancelNotifications()
{
    if (notifyId) {
        gconf_client_notify_remove(client, notifyId);
        notifyId = 0;
    }
}

void MGConfGroupPrivate::notify(GConfClient *, guint cnxn_id, GConfEntry *entry, gpointer user_data)
{
    MGConfGroupPrivate * const priv = static_cast<MGConfGroupPrivate *>(user_data);
    if (cnxn_id != priv->notifyId)
        return;

    const QByteArray key = gconf_entry_get_key(entry);
    const int pathLength = key.lastIndexOf('/');
    if (pathLength + 1 == priv->absolutePath.count()
            && key.startsWith(priv->absolutePath)) {
        const QMetaObject *const metaObject = priv->group->metaObject();
        priv->readPropertyIndex = metaObject->indexOfProperty(key.mid(pathLength + 1));
        if (priv->readPropertyIndex >= priv->propertyOffset) {
            priv->readValue(
                        metaObject->property(priv->readPropertyIndex),
                        gconf_entry_get_value(entry));
        }
        priv->readPropertyIndex = -1;
    }
}
