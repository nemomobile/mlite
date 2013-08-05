#ifndef MLITE_TESTS_TESTBASE_H
#define MLITE_TESTS_TESTBASE_H

#include <QTest>
#include <QtCore/QTimer>

namespace Tests {

class TestBase : public QObject
{
    // intentionally omitted
    //Q_OBJECT

    static const int SIGNAL_WAIT_TIMEOUT = 5000; // ms

protected:
    static bool waitForSignal(QObject *sender, const char *signal);
};

inline bool TestBase::waitForSignal(QObject *sender, const char *signal)
{
    QEventLoop loop;
    connect(sender, signal, &loop, SLOT(quit()));

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timeoutTimer.start(SIGNAL_WAIT_TIMEOUT);

    loop.exec();

    return timeoutTimer.isActive();
}

} // namespace Tests

#endif // MLITE_TESTS_TESTBASE_H
