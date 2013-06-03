/***************************************************************************
** This file was derived from the MDesktopEntry implementation in the
** libmeegotouch library.
**
** Original Copyright:
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Copyright on new work:
** Copyright 2011 Intel Corp.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "mgconfitem.h"

#include <QString>
#include <QVariant>

#include <gconf/gconf-value.h>
#include <gconf/gconf-client.h>

struct MGConfItemPrivate {
    MGConfItemPrivate() :
        notify_id(0),
        have_gconf(false)
    {}

    QString key;
    QVariant value;
    guint notify_id;
    bool have_gconf;

    static QVariant toVariant(GConfValue *value);
    static GConfValue *fromVariant(const QVariant &variant);
    static GConfClient *client();

    static void notify_trampoline(GConfClient *, guint, GConfEntry *, gpointer);

};
