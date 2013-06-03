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

#ifndef MGCONFGROUP_H
#define MGCONFGROUP_H

#include <QObject>

class MGConfGroupPrivate;
class MGConfGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(MGConfGroup *scope READ scope WRITE setScope NOTIFY scopeChanged)
public:
    enum MetaObjectResolution { ImmediateResolve, DeferredResolve };
    MGConfGroup(QObject *parent = 0, MetaObjectResolution resolution = ImmediateResolve);
    ~MGConfGroup();

    QString path() const;
    void setPath(const QString &path);

    MGConfGroup *scope() const;
    void setScope(MGConfGroup *group);

public slots:
    void clear();

signals:
    void pathChanged();
    void scopeChanged();

protected:
    void resolveMetaObject(int propertyOffset);

private slots:
    void propertyChanged();

private:
    friend class MGConfGroupPrivate;
    MGConfGroupPrivate *priv;
};

#endif
