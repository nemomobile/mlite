#include <QtTest/QSignalSpy>

#include "mremoteaction.h"

#include "dbusclienttestbase.h"

namespace Tests {

class UtMRemoteAction : public DBusClientTestBase
{
    Q_OBJECT

public:
    class ServiceMock;

private:
    static const char *const SERVICE_NAME;
    static const char *const OBJECT_PATH;
    static const char *const INTERFACE;

public:
    UtMRemoteAction();

private slots:
    void initTestCase();
    void serialization();
    void trigger();
};

class UtMRemoteAction::ServiceMock : public DBusClientTestBase::MockBase
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.foo.Bar")

public:
    ServiceMock()
        : MockBase(SERVICE_NAME, OBJECT_PATH)
    {
    }

public:
    Q_SCRIPTABLE void Baz(int a1, const QString &a2, const QStringList &a3)
    {
        emit BazCalled(a1, a2, a3);
    }

signals:
    Q_SCRIPTABLE void BazCalled(int a1, const QString &a2, const QStringList &a3);
};

} // namespace Tests

using namespace Tests;

const char *const UtMRemoteAction::SERVICE_NAME = "org.foo.bar";
const char *const UtMRemoteAction::OBJECT_PATH = "/org/foo/bar";
const char *const UtMRemoteAction::INTERFACE = "org.foo.Bar";

UtMRemoteAction::UtMRemoteAction()
{
}

void UtMRemoteAction::initTestCase()
{
    QVERIFY(waitForService(SERVICE_NAME, OBJECT_PATH, INTERFACE));
}

void UtMRemoteAction::serialization()
{
    MRemoteAction original(SERVICE_NAME, OBJECT_PATH, INTERFACE, "Baz",
            QList<QVariant>()
            << 123
            << "abc"
            << (QStringList() << "a" << "b" << "c"));
    MRemoteAction copy1(original.toString()); // conversion constructor
    MRemoteAction copy2(copy1);               // copy constructor

    QCOMPARE(original.toString(), copy1.toString());
    QCOMPARE(original.toString(), copy2.toString());
}

void UtMRemoteAction::trigger()
{
    MRemoteAction action(SERVICE_NAME, OBJECT_PATH, INTERFACE, "Baz",
            QList<QVariant>()
            << 123
            << "abc"
            << (QStringList() << "a" << "b" << "c"));

    QDBusInterface service(SERVICE_NAME, OBJECT_PATH, INTERFACE);
    QVERIFY(service.isValid());

    QSignalSpy spy(&service, SIGNAL(BazCalled(int,QString,QStringList)));

    action.trigger();

    QVERIFY(waitForSignal(&service, SIGNAL(BazCalled(int,QString,QStringList))));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toInt(), 123);
    QCOMPARE(spy[0][1].toString(), QString("abc"));
    QCOMPARE(spy[0][2].toStringList(), QStringList() << "a" << "b" << "c");
}

TEST_MAIN_WITH_MOCK(UtMRemoteAction, UtMRemoteAction::ServiceMock)

#include "ut_mremoteaction.moc"
