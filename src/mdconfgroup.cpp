/***************************************************************************
** Copyright (C) 2014 Jolla Mobile <andrew.den.exter@jollamobile.com>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "mdconf_p.h"
#include "mdconfgroup.h"

#include <QDebug>
#include <QMetaProperty>
#include <QCoreApplication>

class MDConfGroupPrivate : public QObject
{
public:
    MDConfGroupPrivate()
        : group(0)
        , scope(0)
        , client(0)
        , notifyIndex(-1)
        , propertyOffset(-1)
        , signalId(-1)
        , synchronous(false)
    {
    }

    void readValue(const QMetaProperty &property);

    void resolveProperties(const QByteArray &scopePath);
    void cancelNotifications();

    void connectToClient();
    void disconnectFromClient();

    void notify(const QByteArray &basePath, const QByteArray &key);
    static void changed(DConfClient *, gchar *prefix, GStrv changes, gchar *, gpointer user_data);
    void notify(const char *prefix, GStrv changes);
    void customEvent(QEvent* event);

    QByteArray absolutePath;
    QString path;
    QList<MDConfGroup *> children;
    MDConfGroup *group;
    MDConfGroup *scope;

    DConfClient *client;

    int notifyIndex;
    int propertyOffset;
    int signalId;
    bool synchronous;
};

MDConfGroup::MDConfGroup(QObject *parent, BindOption option)
    : QObject(parent)
    , priv(new MDConfGroupPrivate)
{
    priv->group = this;
    if (option == DontBindProperties)
        resolveMetaObject(metaObject()->propertyCount());
}

MDConfGroup::MDConfGroup(const QString &path, QObject *parent, BindOption option)
    : QObject(parent)
    , priv(new MDConfGroupPrivate)
{
    priv->group = this;
    priv->path = path;
    if (option == DontBindProperties)
        resolveMetaObject(metaObject()->propertyCount());
}

MDConfGroup::~MDConfGroup()
{
    if (priv->client) {
        priv->cancelNotifications();

        foreach (MDConfGroup *child, priv->children)
            child->priv->scope = 0;

        // If an absolute path disconnect the signal handler and unref, otherwise just unref.
        if (priv->path.startsWith(QLatin1Char('/')))
            priv->disconnectFromClient();
        else
            g_object_unref(priv->client);
    }

    if (priv->scope)
        priv->scope->priv->children.removeAll(this);
}

void MDConfGroup::resolveMetaObject(int propertyOffset)
{
    // Abort if this has already been called.
    if (priv->propertyOffset >= 0)
        return;

    const int propertyChangedIndex = staticMetaObject.indexOfMethod("propertyChanged()");
    Q_ASSERT(propertyChangedIndex != -1);

    const QMetaObject * const metaObject = this->metaObject();

    // If no property offset is supplied include all the properties of the derived type.
    if (propertyOffset < 0)
        propertyOffset = staticMetaObject.propertyCount();
    priv->propertyOffset = propertyOffset;

    // Connect all the notify signals of the derived type's properties to the propertyChanged()
    // slot.
    for (int i = propertyOffset; i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);

        if (property.hasNotifySignal()) {
            QMetaObject::connect(
                        this,
                        property.notifySignalIndex(),
                        this,
                        propertyChangedIndex,
                        Qt::UniqueConnection);
        }
    }

    // If the path is absolute connect to the change signal and read initial values, otherwise
    // if a parent scope is set and resolved and the path is relative then just read the initial
    // values,
    if (priv->path.startsWith(QLatin1Char('/'))) {
        Q_ASSERT(!priv->client);
        priv->connectToClient();
        priv->resolveProperties(QByteArray());
    } else if (priv->scope && !priv->path.isEmpty() && !priv->scope->priv->absolutePath.isEmpty()) {
        priv->resolveProperties(priv->scope->priv->absolutePath);
    }
}

QString MDConfGroup::path() const
{
    return priv->path;
}

void MDConfGroup::setPath(const QString &path)
{
    if (priv->path != path) {
        const bool wasAbsolute = priv->path.startsWith(QLatin1Char('/'));
        const bool isAbsolute = path.startsWith(QLatin1Char('/'));

        // If listening for notications on the previous path stop.
        if (priv->client && !priv->absolutePath.isEmpty()) {
            priv->cancelNotifications();

            // If changing from an an absolute path to a relative path disconnect from the changed
            // signal, else if changing from a relative path to an absolute path release the
            // reference on the parent scope's client.
            if (wasAbsolute && !isAbsolute) {
                priv->disconnectFromClient();
            } else if (!wasAbsolute) {
                g_object_unref(priv->client);
                priv->client = 0;
            }
        }

        priv->path = path;
        emit pathChanged();

        // If the new path is absolute connect to the changed signal and read the initial values,
        // if it's relative and the parent scope is valid just read the initial values.
        if (priv->path.isEmpty() || priv->propertyOffset < 0) {
            // No path or the object is not yet resolved, so don't do any of the following things.
        } else if (isAbsolute) {
            if (!priv->client)
                priv->connectToClient();
            priv->resolveProperties(QByteArray());
        } else if (priv->scope && !priv->scope->priv->absolutePath.isEmpty()) {
            priv->resolveProperties(priv->scope->priv->absolutePath);
        }
    }
}

MDConfGroup *MDConfGroup::scope() const
{
    return priv->scope;
}

void MDConfGroup::setScope(MDConfGroup *scope)
{
    if (scope != priv->scope) {
        const bool isAbsolute = priv->path.startsWith(QLatin1Char('/'));

        if (priv->scope)
            priv->scope->priv->children.removeAll(this);

        // If the path is not absolute cancel notifications and release the reference to the
        // parent scope's client.
        if (priv->client && !isAbsolute) {
            if (!priv->absolutePath.isEmpty())
                priv->cancelNotifications();
            g_object_unref(priv->client);
            priv->client = 0;
        }

        priv->scope = scope;

        if (priv->scope) {
            priv->scope->priv->children.append(this);

            // If the path is relative and the new scope is valid read the initial values.
            if (!priv->path.isEmpty() && !isAbsolute && !priv->scope->priv->absolutePath.isEmpty()) {
                priv->resolveProperties(priv->scope->priv->absolutePath);
            }
        }

        emit scopeChanged();
    }
}

bool MDConfGroup::isSynchronous() const
{
    return priv->synchronous;
}

void MDConfGroup::setSynchronous(bool synchronous)
{
    if (priv->synchronous != synchronous) {
        priv->synchronous = synchronous;

        if (!priv->absolutePath.isEmpty()) {
            Q_ASSERT(priv->client);
            // If switching to synchronous mode, synchronize and writes made in asynchronous
            // mode.
            if (synchronous)
                MDConf::sync(priv->client);
            // Remove previous watch and replace with one appropriate for the current mode.
            MDConf::unwatch(priv->client, priv->absolutePath, !synchronous);
            MDConf::watch(priv->client, priv->absolutePath, synchronous);
        }
    }
}

QVariant MDConfGroup::value(const QString &key, const QVariant &defaultValue, int typeHint) const
{
    if (!priv->client || priv->absolutePath.isEmpty() || key.isEmpty())
        return defaultValue;

    const QByteArray absoluteKey = !key.startsWith(QLatin1Char('/'))
            ? priv->absolutePath + key.toUtf8()
            : key.toUtf8();
    const QVariant value = MDConf::read(priv->client, absoluteKey, typeHint);
    return value.isValid() ? value : defaultValue;
}

void MDConfGroup::setValue(const QString &key, const QVariant &value)
{
    if (!priv->client || priv->absolutePath.isEmpty() || key.isEmpty())
        return;

    const QByteArray absoluteKey = !key.startsWith(QLatin1Char('/'))
            ? priv->absolutePath + key.toUtf8()
            : key.toUtf8();
    MDConf::write(priv->client, absoluteKey, value, priv->synchronous);
}

void MDConfGroup::clear()
{
    if (priv->client)
        MDConf::clear(priv->client, priv->absolutePath, priv->synchronous);
}

void MDConfGroup::sync()
{
    if (priv->client)
        MDConf::sync(priv->client);
}

void MDConfGroup::propertyChanged()
{
    const int notifyIndex = senderSignalIndex();

    if (priv->absolutePath.isEmpty() || notifyIndex == priv->notifyIndex)
        return;

    const QMetaObject * const metaObject = this->metaObject();

    for (int i = priv->propertyOffset; i < metaObject->propertyCount(); ++i) {
        const QMetaProperty property = metaObject->property(i);
        if (property.notifySignalIndex() == notifyIndex) {
            MDConf::write(
                        priv->client,
                        priv->absolutePath + property.name(),
                        property.read(this),
                        priv->synchronous);
        }
    }
}

void MDConfGroupPrivate::connectToClient()
{
    Q_ASSERT(!client);
    client = MDConf::client();
    signalId = g_signal_connect(client, "changed", G_CALLBACK(changed), this);
}

void MDConfGroupPrivate::disconnectFromClient()
{
    Q_ASSERT(client);
    g_signal_handler_disconnect(client, signalId);
    if (!synchronous)
        MDConf::sync(client);
    g_object_unref(client);
    client = 0;
}

void MDConfGroupPrivate::readValue(const QMetaProperty &property)
{
    const QVariant value = MDConf::read(client, absolutePath + property.name(), property.type());
    if (value.isValid()) {
        // Record the notify signal index so the propertyChanged() slot knows the change
        // is because of a read from dconf.
        notifyIndex = property.notifySignalIndex();
        property.write(group, value);
        notifyIndex = -1;
    }
}

void MDConfGroupPrivate::resolveProperties(const QByteArray &scopePath)
{
    // If a relative group grab a reference to the parent group's client.
    if (!scopePath.isEmpty()) {
        Q_ASSERT(scope);
        Q_ASSERT(!client);
        Q_ASSERT(scope->priv->client);

        client = scope->priv->client;
        g_object_ref(client);
    }

    // Construct the absolute directory path for this group.
    absolutePath = scopePath + path.toUtf8() + '/';

    // Iterate over the object's properties and read the corresponding values from dconf
    // and write them to the property.
    const QMetaObject * const metaObject = group->metaObject();
    for (int i = propertyOffset; i < metaObject->propertyCount(); ++i)
        readValue(metaObject->property(i));

    MDConf::watch(client, absolutePath, synchronous);

    // Recurse into children with relative paths and resolve their properties as well.
    for (int i = 0; i < children.count(); ++i) {
        MDConfGroup * const child = children.at(i);
        if (child->priv->absolutePath.isEmpty()
                && !child->priv->path.isEmpty()
                && !child->priv->path.startsWith(QLatin1Char('/'))) {
            children.at(i)->priv->resolveProperties(absolutePath);
        }
    }
}

void MDConfGroupPrivate::cancelNotifications()
{
    if (!absolutePath.isEmpty()) {
        MDConf::unwatch(client, absolutePath, synchronous);
        absolutePath = QByteArray();

        // Recurse into any watching child items with relative paths and cancel their
        // notifications as well.
        foreach (MDConfGroup *child, children) {
            if (!child->priv->path.startsWith("/") && child->priv->client) {
                child->priv->cancelNotifications();
                g_object_unref(child->priv->client);
                child->priv->client = 0;
            }
        }
    }
}

void MDConfGroupPrivate::notify(const QByteArray &basePath, const QByteArray &key)
{
    // If the notification is for this group read the new value for a specify property or
    // if key is empty all properties.  Otherwise pass the notification onto any child groups
    // whose absolute path is prefix of the notification path.
    if (basePath.length() == absolutePath.length()) {
        const QMetaObject *const metaObject = group->metaObject();
        if (!key.isEmpty()) {
            const int propertyIndex = metaObject->indexOfProperty(key);
            if (propertyIndex >= propertyOffset)
                readValue(metaObject->property(propertyIndex));
            emit group->valueChanged(QString::fromUtf8(key));
        } else {
            for (int i = propertyOffset; i < metaObject->propertyCount(); ++i)
                readValue(metaObject->property(i));
            emit group->valuesChanged();
        }
    } else for (int i = 0; i < children.count(); ++i) {
        MDConfGroup * const child = children.at(i);
        if (!child->priv->path.startsWith(QLatin1Char('/'))
                && basePath.startsWith(child->priv->absolutePath)) {
            child->priv->notify(basePath, key);
        }
    }
}

void MDConfGroupPrivate::notify(const char *prefix, GStrv changes)
{
    const QByteArray prefixPath = QByteArray(prefix);
    // Early exit if the notification is not relevant to this group.
    if (!prefixPath.startsWith(absolutePath) && !absolutePath.startsWith(prefixPath))
        return;

    // Construct the group path and key for each change and perform updates.
    for (int i = 0; changes[i]; ++i) {
        const QByteArray absoluteKey = prefixPath + QByteArray(changes[i]);
        const int pathLength = absoluteKey.lastIndexOf('/') + 1;
        const QByteArray basePath = absoluteKey.mid(0, pathLength);
        const QByteArray key = absoluteKey.mid(pathLength);

        notify(basePath, key);
    }
}

void MDConfGroupPrivate::customEvent(QEvent* event)
{
    if (event->type() == (QEvent::Type)MDConf::Event::TYPE) {
        MDConf::Event* dconfEvent = (MDConf::Event*)event;
        notify(dconfEvent->prefix, dconfEvent->changes);
    }
}

void MDConfGroupPrivate::changed(DConfClient *, gchar *prefix, GStrv changes, gchar *, gpointer data)
{
    MDConf::Event event(prefix, changes);
    QCoreApplication::sendEvent((MDConfGroupPrivate*)data, &event);
}
