#ifndef MLITE_TESTS_DBUSCLIENTTESTBASE_H
#define MLITE_TESTS_DBUSCLIENTTESTBASE_H

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusServiceWatcher>

#include "testbase.h"

namespace Tests {

class DBusClientTestBase : public TestBase
{
    // intentionally omitted
    //Q_OBJECT

public:
    class MockBase;

public:
    DBusClientTestBase();

protected:
    static bool waitForService(const QString &serviceName, const QString &path,
            const QString &interface);
    static QDBusConnection bus() { return QDBusConnection::sessionBus(); }
};

class DBusClientTestBase::MockBase : public QObject
{
public:
    MockBase(const QString &serviceName, const QString &path)
    {
        if (!bus().registerObject(path, this, QDBusConnection::ExportScriptableContents)) {
            qFatal("Failed to register mock D-Bus object at path '%s': '%s'",
                    qPrintable(path), qPrintable(bus().lastError().message()));
        }

        if (!bus().registerService(serviceName)) {
            qFatal("Failed to register mock D-Bus service '%s': '%s'",
                    qPrintable(serviceName), qPrintable(bus().lastError().message()));
        }
    }

public:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    static void msgHandler(QtMsgType type, const char *message)
    {
        qInstallMsgHandler(0);
        qt_message_output(type, QByteArray("MOCK   : ").append(message));
        qInstallMsgHandler(msgHandler);
    }

    static void installMsgHandler()
    {
      qInstallMsgHandler(msgHandler);
    }
#else
    static void messageHandler(QtMsgType type, const QMessageLogContext &context,
        const QString &message)
    {
        qInstallMessageHandler(0);
        qt_message_output(type, context, QString("MOCK   : ").append(message));
        qInstallMessageHandler(messageHandler);
    }

    static void installMsgHandler()
    {
      qInstallMessageHandler(messageHandler);
    }
#endif
};

inline DBusClientTestBase::DBusClientTestBase()
{
}

inline bool DBusClientTestBase::waitForService(const QString &serviceName, const QString &path,
        const QString &interface)
{
    if (QDBusInterface(serviceName, path, interface, bus()).isValid()) {
        return true;
    }

    QDBusServiceWatcher watcher(serviceName, bus(), QDBusServiceWatcher::WatchForRegistration);
    return waitForSignal(&watcher, SIGNAL(serviceRegistered(QString)));
}

#define TEST_MAIN_WITH_MOCK(TestClass, MockClass)                                                 \
    int main(int argc, char *argv[])                                                              \
    {                                                                                             \
        if (argc == 2 && argv[1] == QLatin1String("--mock")) {                                    \
            Tests::DBusClientTestBase::MockBase::installMsgHandler();                             \
            QCoreApplication app(argc, argv);                                                     \
                                                                                                  \
            qDebug("%s: starting...", Q_FUNC_INFO);                                               \
                                                                                                  \
            if (qgetenv("DBUS_SESSION_BUS_ADDRESS").isEmpty()) {                                  \
                qFatal("DBUS_SESSION_BUS_ADDRESS is empty. Session bus not available. Aborting.");\
            }                                                                                     \
                                                                                                  \
            MockClass mock;                                                                       \
                                                                                                  \
            return app.exec();                                                                    \
        } else {                                                                                  \
            QCoreApplication app(argc, argv);                                                     \
                                                                                                  \
            TestClass test;                                                                       \
                                                                                                  \
            QProcess mock;                                                                        \
            mock.setProcessChannelMode(QProcess::ForwardedChannels);                              \
            mock.start(app.applicationFilePath(), QStringList("--mock"));                         \
            if (mock.state() == QProcess::NotRunning) {                                           \
                qFatal("Failed to start mock");                                                   \
            }                                                                                     \
                                                                                                  \
            const int retv = QTest::qExec(&test, argc, argv);                                     \
                                                                                                  \
            mock.terminate();                                                                     \
            mock.waitForFinished();                                                               \
                                                                                                  \
            return retv;                                                                          \
        }                                                                                         \
    }                                                                                             \

} // namespace Tests

#endif // MLITE_TESTS_DBUSCLIENTTESTBASE_H
