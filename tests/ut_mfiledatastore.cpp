#include <QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QDir>

#include "mfiledatastore.h"

namespace Tests {

class UtMFileDataStore : public QObject
{
    Q_OBJECT

public:
    UtMFileDataStore();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void basicTest();
    void setUnknownValue();
    void readValue();
    void createValue();
    void setKnownValue();
    void removeUnknownValue();
    void removeKnownValue();
    void createThreeValues();
    void removeOneValue();
    void clear();
    void addAndClear();
    void addAndClearImmediately();
    void otherProcessCreateValue();
    void otherProcessSetValue();
    void otherProcessCreateOtherValue();
    void otherProcessSetAndRemoveValue();

private:
    static QString filePath();
    static bool writeFile(const QByteArray &data);
};

} // namespace Tests

using namespace Tests;

UtMFileDataStore::UtMFileDataStore()
{
}

void UtMFileDataStore::initTestCase()
{
    QVERIFY(QDir().mkpath(QFileInfo(filePath()).absolutePath()));
}

void UtMFileDataStore::cleanupTestCase()
{
    const QString tmpDir = QFileInfo(filePath()).absolutePath();
    if (QDir(tmpDir).count() == 0) {
        QDir().rmdir(tmpDir);
    }
}

void UtMFileDataStore::init()
{
    QFile file(filePath());
    if (file.exists() && !file.remove()) {
        QFAIL("Failed to remove temporary file");
    }
}

void UtMFileDataStore::cleanup()
{
    if (!QTest::currentTestFailed()) {
        QFile(filePath()).remove();
    }
}

void UtMFileDataStore::basicTest()
{
    MFileDataStore store1(filePath());
    QVERIFY(store1.isReadable());
    QVERIFY(store1.isWritable());
    MFileDataStore store2(filePath());
    QVERIFY(store2.isReadable());
    QVERIFY(store2.isWritable());
}

void UtMFileDataStore::setUnknownValue()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(!store1.setValue("key-does-not-exist", "foo"));
    QVERIFY(!store1.contains("key-does-not-exist"));
    QVERIFY(!store2.contains("key-does-not-exist"));
}

void UtMFileDataStore::readValue()
{
    if (!writeFile("[General]\nfoo=bar\n")) {
        QFAIL("Failed to write file");
    }

    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.contains("foo"));
    QCOMPARE(store1.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store1.value("foo").toString(), QString("bar"));
    QVERIFY(store2.contains("foo"));
    QCOMPARE(store2.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store2.value("foo").toString(), QString("bar"));
}

void UtMFileDataStore::createValue()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(!store1.contains("foo"));
    QVERIFY(!store2.contains("foo"));

    QVERIFY(store1.createValue("foo", "bar"));

    QVERIFY(store1.contains("foo"));
    QCOMPARE(store1.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store1.value("foo").toString(), QString("bar"));
    QVERIFY(store2.contains("foo"));
    QCOMPARE(store2.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store2.value("foo").toString(), QString("bar"));
}

void UtMFileDataStore::setKnownValue()
{
    if (!writeFile("[General]\nfoo=bar\n")) {
        QFAIL("Failed to write file");
    }

    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.setValue("foo", "baz"));

    QVERIFY(store1.contains("foo"));
    QCOMPARE(store1.allKeys(), QStringList() << "foo");
    QCOMPARE(store1.value("foo").toString(), QString("baz"));
    QVERIFY(store2.contains("foo"));
    QCOMPARE(store2.allKeys(), QStringList() << "foo");
    QCOMPARE(store2.value("foo").toString(), QString("baz"));
}

void UtMFileDataStore::removeUnknownValue()
{
    MFileDataStore store1(filePath());

    store1.remove("bar");

    // nothing to check
}

void UtMFileDataStore::removeKnownValue()
{
    if (!writeFile("[General]\nfoo=bar\n")) {
        QFAIL("Failed to write file");
    }

    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    store1.remove("foo");

    QVERIFY(!store1.contains("foo"));
    QVERIFY(store1.allKeys().isEmpty());
    QVERIFY(!store2.contains("foo"));
    QVERIFY(store2.allKeys().isEmpty());
}

void UtMFileDataStore::createThreeValues()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.createValue("foo", "fooVal"));
    QVERIFY(store1.createValue("bar", "barVal"));
    QVERIFY(store1.createValue("baz", "bazVal"));

    QStringList allKeys = QStringList() << "foo" << "bar" << "baz";
    allKeys.sort();

    QCOMPARE(store1.allKeys(), allKeys);
    QCOMPARE(store1.value("foo").toString(), QString("fooVal"));
    QCOMPARE(store1.value("bar").toString(), QString("barVal"));
    QCOMPARE(store1.value("baz").toString(), QString("bazVal"));
    QCOMPARE(store2.allKeys(), allKeys);
    QCOMPARE(store2.value("foo").toString(), QString("fooVal"));
    QCOMPARE(store2.value("bar").toString(), QString("barVal"));
    QCOMPARE(store2.value("baz").toString(), QString("bazVal"));
}

void UtMFileDataStore::removeOneValue()
{
    if (!writeFile("[General]\nfoo=fooVal\nbar=barVal\nbaz=bazVal\n")) {
        QFAIL("Failed to write file");
    }

    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    store1.remove("bar");

    QStringList allKeys = QStringList() << "foo" << "baz";
    allKeys.sort();

    QVERIFY(!store1.contains("bar"));
    QCOMPARE(store1.allKeys(), allKeys);
    QVERIFY(!store2.contains("bar"));
    QCOMPARE(store2.allKeys(), allKeys);
}

void UtMFileDataStore::clear()
{
    if (!writeFile("[General]\nfoo=bar\n")) {
        QFAIL("Failed to write file");
    }

    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    store1.clear();

    QVERIFY(store1.allKeys().isEmpty());
    QVERIFY(store2.allKeys().isEmpty());
}

void UtMFileDataStore::addAndClear()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.createValue("foo", "bar"));

    QCoreApplication::processEvents();

    store1.clear();

    QVERIFY(store1.allKeys().isEmpty());
    QVERIFY(store2.allKeys().isEmpty());
}

void UtMFileDataStore::addAndClearImmediately()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.createValue("foo", "bar"));

    // Intentionally do not QCoreApplication::processEvents() here

    store1.clear();

    QVERIFY(store1.allKeys().isEmpty());
    QVERIFY(store2.allKeys().isEmpty());
}

void UtMFileDataStore::otherProcessCreateValue()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(!store1.contains("foo"));

    if (!writeFile("[General]\nfoo=bar\n")) {
        QFAIL("Failed to write file");
    }

    QCoreApplication::processEvents();

    QVERIFY(store1.contains("foo"));
    QCOMPARE(store1.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store1.value("foo").toString(), QString("bar"));
    QVERIFY(store2.contains("foo"));
    QCOMPARE(store2.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store2.value("foo").toString(), QString("bar"));
}

void UtMFileDataStore::otherProcessSetValue()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.createValue("foo", "bar"));

    QCoreApplication::processEvents();

    // it is important to set value of different length - QSettings only reload file if its
    // size or timestamp differs!
    if (!writeFile("[General]\nfoo=notbar\n")) {
        QFAIL("Failed to write file");
    }

    QCoreApplication::processEvents();

    QVERIFY(store1.contains("foo"));
    QCOMPARE(store1.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store1.value("foo").toString(), QString("notbar"));
    QVERIFY(store2.contains("foo"));
    QCOMPARE(store2.allKeys(), QStringList() <<  "foo");
    QCOMPARE(store2.value("foo").toString(), QString("notbar"));
}

void UtMFileDataStore::otherProcessCreateOtherValue()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.createValue("foo", "bar"));

    QCoreApplication::processEvents();

    if (!writeFile("[General]\nfoo=bar\nbar=baz\n")) {
        QFAIL("Failed to write file");
    }

    QCoreApplication::processEvents();

    QStringList allKeys = QStringList() << "foo" << "bar";
    allKeys.sort();

    QVERIFY(store1.contains("bar"));
    QCOMPARE(store1.allKeys(), allKeys);
    QCOMPARE(store1.value("foo").toString(), QString("bar"));
    QCOMPARE(store1.value("bar").toString(), QString("baz"));
    QVERIFY(store2.contains("bar"));
    QCOMPARE(store2.allKeys(), allKeys);
    QCOMPARE(store2.value("foo").toString(), QString("bar"));
    QCOMPARE(store2.value("bar").toString(), QString("baz"));
}

void UtMFileDataStore::otherProcessSetAndRemoveValue()
{
    MFileDataStore store1(filePath());
    MFileDataStore store2(filePath());

    QVERIFY(store1.createValue("foo", "bar"));
    QVERIFY(store1.createValue("bar", "baz"));

    QCoreApplication::processEvents();

    // it is important to set value of different length - QSettings only reload file if its
    // size or timestamp differs!
    if (!writeFile("[General]\nfoo=baz\nbaz=nobar\n")) {
        QFAIL("Failed to write file");
    }

    QCoreApplication::processEvents();

    QStringList allKeys = QStringList() << "foo" << "baz";
    allKeys.sort();

    QVERIFY(!store1.contains("bar"));
    QVERIFY(store1.contains("baz"));
    QCOMPARE(store1.allKeys(), allKeys);
    QCOMPARE(store1.value("foo").toString(), QString("baz"));
    QCOMPARE(store1.value("baz").toString(), QString("nobar"));
    QVERIFY(!store2.contains("bar"));
    QVERIFY(store2.contains("baz"));
    QCOMPARE(store2.allKeys(), allKeys);
    QCOMPARE(store2.value("foo").toString(), QString("baz"));
    QCOMPARE(store2.value("baz").toString(), QString("nobar"));
}

QString UtMFileDataStore::filePath()
{
    return QDir::temp().filePath(QString("ut_mfiledatastore/%1.ini")
            .arg(QTest::currentTestFunction()));
}

bool UtMFileDataStore::writeFile(const QByteArray &data)
{
    QFile file(filePath());

    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        return false;
    }

    if (file.write(data) != data.size()) {
        return false;
    }

    file.close();

    return true;
}

QTEST_MAIN(Tests::UtMFileDataStore)

#include "ut_mfiledatastore.moc"
