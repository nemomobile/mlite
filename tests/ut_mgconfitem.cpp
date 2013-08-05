#include <QtCore/QProcess>
#include <QtTest/QSignalSpy>
#include <QTest>

#include "mgconfitem.h"

#include "testbase.h"

namespace Tests {

class UtMGConfItem : public TestBase
{
    Q_OBJECT

public:
    UtMGConfItem();

private slots:
    void initTestCase();
    void cleanup();
    void basicTest_data();
    void basicTest();
    void defaultValue();

private:
    static const QString KEY;
};

} // namespace Tests

using namespace Tests;

const QString UtMGConfItem::KEY = "/mlite-tests/ut_mgconfitem/foo";

UtMGConfItem::UtMGConfItem()
{
}

void UtMGConfItem::initTestCase()
{
    MGConfItem(KEY).unset();
}

void UtMGConfItem::cleanup()
{
    if (!QTest::currentTestFailed()) {
        MGConfItem(KEY).unset();
    }
}

void UtMGConfItem::basicTest_data()
{
    QTest::addColumn<QVariant>("value1");
    QTest::addColumn<QVariant>("value2");

    QTest::newRow("string")  << QVariant("bar") << QVariant("baz");
    QTest::newRow("bool") << QVariant(true) << QVariant(false);
    QTest::newRow("int") << QVariant(42) << QVariant(24);
    QTest::newRow("float") << QVariant((double)42.5) << QVariant((double)24.5);
    QTest::newRow("string-list")
        << QVariant((QStringList() << "bar" << "baz"))
        << QVariant((QStringList() << "baz" << "bar"));
    QTest::newRow("variant-list-string")
        << QVariant((QVariantList() << "bar" << "baz"))
        << QVariant((QVariantList() << "baz" << "bar"));
    QTest::newRow("variant-list-bool")
        << QVariant((QVariantList() << true << false))
        << QVariant((QVariantList() << false << true));
    QTest::newRow("variant-list-int")
        << QVariant((QVariantList() << 42 << 24))
        << QVariant((QVariantList() << 24 << 42));
    QTest::newRow("variant-list-float")
        << QVariant((QVariantList() << (double)42.5 << (double)24.5))
        << QVariant((QVariantList() << (double)24.5 << (double)42.5));
}

void UtMGConfItem::basicTest()
{
    QFETCH(QVariant, value1);
    QFETCH(QVariant, value2);

    MGConfItem item1(KEY);

    MGConfItem item2(KEY);
    QSignalSpy spy2(&item2, SIGNAL(valueChanged()));
    item2.set(value1);
    QCOMPARE(item2.value(), value1);
    QCOMPARE(spy2.count(), 1);

    waitForSignal(&item1, SIGNAL(valueChanged()));
    QCOMPARE(item1.value(), value1);

    QCOMPARE(MGConfItem(KEY).value(), value1);

    QSignalSpy spy1(&item1, SIGNAL(valueChanged()));
    item1.set(value2);
    QCOMPARE(item1.value(), value2);
    QCOMPARE(spy1.count(), 1);

    waitForSignal(&item2, SIGNAL(valueChanged()));
    QCOMPARE(item2.value(), value2);

    QCOMPARE(MGConfItem(KEY).value(), value2);
}

void UtMGConfItem::defaultValue()
{
    MGConfItem(KEY).unset();
    QCOMPARE(MGConfItem(KEY).value("default").toString(), QString("default"));
}

QTEST_MAIN(Tests::UtMGConfItem)

#include "ut_mgconfitem.moc"
