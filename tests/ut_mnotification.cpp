#include <QtDBus/QDBusReply>
#include <QtDBus/QtDBus>

#include "mnotification.h"
#include "metatypedeclarations.h"

#include "dbusclienttestbase.h"

namespace Tests {

class UtMNotification : public DBusClientTestBase
{
    Q_OBJECT

public:
    class ManagerMock;
    struct NotificationData;

public:
    UtMNotification();

private slots:
    void initTestCase();
    void basic();
};

class UtMNotification::ManagerMock : public DBusClientTestBase::MockBase
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
    ManagerMock();

public:
    Q_SCRIPTABLE void CloseNotification(uint id);
    Q_SCRIPTABLE QStringList GetCapabilities();
    Q_SCRIPTABLE QList<Tests::UtMNotification::NotificationData> GetNotifications(
            const QString &appName);
    Q_SCRIPTABLE QString GetServerInformation(QString &name, QString &vendor, QString &version);
    Q_SCRIPTABLE uint Notify(const QString &appName, uint replacesId, const QString &appIcon,
            const QString &summary, const QString &body, const QStringList &actions,
            const QVariantHash &hints, int expireTimeout);

signals:
    Q_SCRIPTABLE void ActionInvoked(uint id, const QString &actionKey);
    Q_SCRIPTABLE void NotificationClosed(uint id, uint reason);

private:
    QMap<uint, NotificationData> m_notifications; // expect sorted by id
    uint m_nextId;
};

// Plain-old-data representation of MNotification
struct UtMNotification::NotificationData
{
    NotificationData()
    {
    }

    NotificationData(const QString &appName, uint id, const QString &appIcon,
            const QString &summary, const QString &body, const QStringList &actions,
            const QVariantHash &hints, int expireTimeout)
        : appName(appName),
          id(id),
          appIcon(appIcon),
          summary(summary),
          body(body),
          actions(actions),
          hints(hints),
          expireTimeout(expireTimeout)
    {
    }

    QString appName;
    uint id;
    QString appIcon;
    QString summary;
    QString body;
    QStringList actions;
    QVariantHash hints;
    int expireTimeout;
};

QDBusArgument &operator<<(QDBusArgument &argument,
        const UtMNotification::NotificationData &notification)
{
    argument.beginStructure();
    argument << notification.appName;
    argument << notification.id;
    argument << notification.appIcon;
    argument << notification.summary;
    argument << notification.body;
    argument << notification.actions;
    argument << notification.hints;
    argument << notification.expireTimeout;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
        UtMNotification::NotificationData &notification)
{
    argument.beginStructure();
    argument >> notification.appName;
    argument >> notification.id;
    argument >> notification.appIcon;
    argument >> notification.summary;
    argument >> notification.body;
    argument >> notification.actions;
    argument >> notification.hints;
    argument >> notification.expireTimeout;
    argument.endStructure();
    return argument;
}

} // namespace Tests

Q_DECLARE_METATYPE(Tests::UtMNotification::NotificationData)
Q_DECLARE_METATYPE(QList<Tests::UtMNotification::NotificationData>)

using namespace Tests;

/*
 * \class Tests::UtMNotification
 */

UtMNotification::UtMNotification()
{
    qDBusRegisterMetaType<UtMNotification::NotificationData>();
    qDBusRegisterMetaType<QList<UtMNotification::NotificationData> >();
}

void UtMNotification::initTestCase()
{
    QVERIFY(waitForService("org.freedesktop.Notifications", "/org/freedesktop/Notifications",
          "org.freedesktop.Notifications"));
}

/*
 * 1. publish notification
 * 2. publish notification group
 * 3. update the notification, add it to the group
 * 4. remove the notification
 * 5. remove the group
 */
void UtMNotification::basic()
{
    QDBusInterface service("org.freedesktop.Notifications", "/org/freedesktop/Notifications",
            "org.freedesktop.Notifications");
    QVERIFY(service.isValid());

    MNotification notification("general", "a-summary", "a-body");
    notification.setIdentifier("an-identifier");
    notification.setImage("an-icon");
    notification.setCount(42);

    QVERIFY(notification.publish());
    QVERIFY(notification.isPublished());

    {
        QList<MNotification *> notifications = MNotification::notifications();
        QCOMPARE(notifications.count(), 1);
        QCOMPARE(notifications.first()->id(), notification.id());
        QCOMPARE(notifications.first()->eventType(), notification.eventType());
        QCOMPARE(notifications.first()->summary(), notification.summary());
        QCOMPARE(notifications.first()->body(), notification.body());
        QCOMPARE(notifications.first()->identifier(), notification.identifier());
        QCOMPARE(notifications.first()->image(), notification.image());
        QCOMPARE(notifications.first()->count(), notification.count());
        qDeleteAll(notifications);
    }

    {
        QDBusReply<QList<NotificationData> > notifications = service.call("GetNotifications", "");
        QCOMPARE(notifications.value().count(), 1);
        QCOMPARE(notifications.value().first().id, notification.id());
        QCOMPARE(notifications.value().first().hints.value("category").toString(),
                notification.eventType());
        QCOMPARE(notifications.value().first().summary, notification.summary());
        QCOMPARE(notifications.value().first().body, notification.body());
        QCOMPARE(notifications.value().first().hints.value("x-nemo-legacy-identifier").toString(),
                notification.identifier());
        QCOMPARE(notifications.value().first().appIcon, notification.image());
        QCOMPARE(notifications.value().first().hints.value("x-nemo-item-count").toUInt(),
                notification.count());
    }

    MNotificationGroup group("specific", "a-group-summary", "a-group-body");
    group.setIdentifier("a-group-identifier");
    group.setImage("a-group-icon");
    group.setCount(420);
    QVERIFY(group.publish());

    {
        QDBusReply<QList<NotificationData> > notifications = service.call("GetNotifications", "");
        // 0 - notification, 1 - notification group
        QCOMPARE(notifications.value().count(), 2);
        QCOMPARE(notifications.value().at(1).id, group.id());
        QCOMPARE(notifications.value().at(1).hints.value("category").toString(),
                group.eventType());
        QCOMPARE(notifications.value().at(1).hints.value("x-nemo-legacy-summary").toString(),
                group.summary());
        QCOMPARE(notifications.value().at(1).hints.value("x-nemo-legacy-body").toString(),
                group.body());
        QCOMPARE(notifications.value().at(1).hints.value("x-nemo-legacy-identifier").toString(),
                group.identifier());
        QCOMPARE(notifications.value().at(1).appIcon, group.image());
        QCOMPARE(notifications.value().at(1).hints.value("x-nemo-item-count").toUInt(),
                group.count());
    }

    notification.setEventType("specific");
    notification.setSummary("a-new-summary");
    notification.setBody("a-new-body");
    notification.setIdentifier("a-new-identifier");
    notification.setImage("a-new-icon");
    notification.setCount(24);
    MRemoteAction action("org.example.Foo", "/org/example/Foo", "org.example.Foo", "Bar",
            QList<QVariant>() << "a" << 0xb);
    notification.setAction(action);
    notification.setGroup(group);

    QVERIFY(notification.publish());
    QVERIFY(notification.isPublished());

    {
        QList<MNotification *> notifications = MNotification::notifications();
        QCOMPARE(notifications.count(), 1);
        QCOMPARE(notifications.first()->id(), notification.id());
        QCOMPARE(notifications.first()->eventType(), notification.eventType());
        QCOMPARE(notifications.first()->summary(), notification.summary());
        QCOMPARE(notifications.first()->body(), notification.body());
        QCOMPARE(notifications.first()->identifier(), notification.identifier());
        QCOMPARE(notifications.first()->image(), notification.image());
        QCOMPARE(notifications.first()->count(), notification.count());
        QCOMPARE(notifications.first()->groupId(), group.id());
        qDeleteAll(notifications);
    }

    {
        QDBusReply<QList<NotificationData> > notifications = service.call("GetNotifications", "");
        // 0 - notification, 1 - notification group
        QCOMPARE(notifications.value().count(), 2);
        QCOMPARE(notifications.value().first().id, notification.id());
        QCOMPARE(notifications.value().first().hints.value("category").toString(),
                notification.eventType());
        QCOMPARE(notifications.value().first().hints.value("x-nemo-legacy-summary").toString(),
                notification.summary());
        QCOMPARE(notifications.value().first().hints.value("x-nemo-legacy-body").toString(),
                notification.body());
        QCOMPARE(notifications.value().first().hints.value("x-nemo-legacy-identifier").toString(),
                notification.identifier());
        QCOMPARE(notifications.value().first().appIcon, notification.image());
        QCOMPARE(notifications.value().first().hints.value("x-nemo-item-count").toUInt(),
                notification.count());
        QCOMPARE(notifications.value().first().hints.value("x-nemo-remote-action-default")
                .toString(), action.toString());
    }

    QVERIFY(notification.remove());

    {
        QDBusReply<QList<NotificationData> > notifications = service.call("GetNotifications", "");
        QCOMPARE(notifications.value().count(), 1);
        QCOMPARE(notifications.value().first().id, group.id());
    }

    QVERIFY(group.remove());

    {
        QDBusReply<QList<NotificationData> > notifications = service.call("GetNotifications", "");
        QCOMPARE(notifications.value().count(), 0);
    }
}

/*
 * \class Tests::UtMNotification::ManagerMock
 */

UtMNotification::ManagerMock::ManagerMock()
    : MockBase("org.freedesktop.Notifications", "/org/freedesktop/Notifications"),
      m_nextId(1)
{
    qDBusRegisterMetaType<UtMNotification::NotificationData>();
    qDBusRegisterMetaType<QList<UtMNotification::NotificationData> >();
}

void UtMNotification::ManagerMock::CloseNotification(uint id)
{
    m_notifications.remove(id);
}

QStringList UtMNotification::ManagerMock::GetCapabilities()
{
    return QStringList()
        << "x-nemo-get-notifications";
}

QList<UtMNotification::NotificationData>
    UtMNotification::ManagerMock::GetNotifications(const QString &appName)
{
    Q_UNUSED(appName);
    return m_notifications.values();
}

QString UtMNotification::ManagerMock::GetServerInformation(QString &name, QString &vendor,
        QString &version)
{
    Q_UNUSED(name);
    Q_UNUSED(vendor);
    Q_UNUSED(version);
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unimplemented!");
    return QString();
}

uint UtMNotification::ManagerMock::Notify(const QString &appName, uint replacesId,
        const QString &appIcon, const QString &summary, const QString &body,
        const QStringList &actions, const QVariantHash &hints, int expireTimeout)
{
    const uint id = replacesId != 0 ? replacesId : m_nextId++;
    NotificationData notification(appName, id, appIcon, summary, body, actions, hints,
            expireTimeout);
    m_notifications[id] = notification;

    return id;
}

TEST_MAIN_WITH_MOCK(UtMNotification, UtMNotification::ManagerMock)

#include "ut_mnotification.moc"
