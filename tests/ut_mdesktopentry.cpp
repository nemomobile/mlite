#include <QTest>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTextStream>

#include "mdesktopentry.h"

namespace Tests {

class UtMDesktopEntry : public QObject
{
    Q_OBJECT

public:
    typedef QMap<QString, QString> Values;

public:
    UtMDesktopEntry();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void basicTest();
    void isValid_data();
    void isValid();
    void contains();
    void value();
    void duplicateKey();
    void values_data();
    void values();
    void localization_data();
    void localization();

private:
    QString createDesktopEntry(const Values &values);

private:
    MDesktopEntry *m_entry;
    QTemporaryFile *m_temporaryFile;
};

} // namespace Tests

using namespace Tests;

Q_DECLARE_METATYPE(UtMDesktopEntry::Values)

UtMDesktopEntry::UtMDesktopEntry()
    : m_entry(0),
      m_temporaryFile(0)
{
    qRegisterMetaType<Values>();
}

void UtMDesktopEntry::initTestCase()
{
}

void UtMDesktopEntry::cleanupTestCase()
{
}

void UtMDesktopEntry::init()
{
}

void UtMDesktopEntry::cleanup()
{
    // cleanup after createDesktopEntry() if used
    if (QTest::currentTestFailed() && m_temporaryFile != 0) {
        m_temporaryFile->setAutoRemove(false);
    }

    delete m_temporaryFile;
    m_temporaryFile = 0;
}

void UtMDesktopEntry::basicTest()
{
    MDesktopEntry entry("/dev/null");
    QCOMPARE(entry.fileName(), QString("/dev/null"));
}

void UtMDesktopEntry::isValid_data()
{
    QTest::addColumn<Values>("values");
    QTest::addColumn<bool>("expected");

    struct GeneralRulesHelper
    {
        static void add(const QString &tag, const QString &type, const QString &name,
                const QString &exec, const QString &url, bool expected)
        {
            Values values;
            if (!type.isNull()) {
                values["Type"] = type;
            }
            if (!name.isNull()) {
                values["Name"] = name;
            }
            if (!exec.isNull()) {
                values["Exec"] = exec;
            }
            if (!url.isNull()) {
                values["URL"] = url;
            }
            QTest::newRow(qPrintable(QString("general_%1").arg(tag))) << values << expected;
        }
    };
    GeneralRulesHelper::add("validApplication", "Application", "Foo", "true", QString(), true);
    GeneralRulesHelper::add("validLink", "Link", "Foo", QString(), "http://www.foo.org", true);
    GeneralRulesHelper::add("noType", QString(), "Foo", "true", QString(), false);
    GeneralRulesHelper::add("emptyType", "", "Foo", "true", QString(), false);
    GeneralRulesHelper::add("noName", "Application", QString(), "true", QString(), false);
    GeneralRulesHelper::add("emptyName", "Application", "", "true", QString(), false);
    GeneralRulesHelper::add("appWithoutExec", "Application", "Foo", QString(), QString(), false);
    GeneralRulesHelper::add("appEmptyExec", "Application", "Foo", "", QString(), false);
    GeneralRulesHelper::add("linkWithoutUrl", "Link", "Foo", QString(), QString(), false);
    GeneralRulesHelper::add("linkEmptyUrl", "Link", "Foo", QString(), "", false);

    const QStringList multivalueKeys = QStringList()
        << "Categories"
        << "OnlyShowIn"
        << "NotShowIn"
        << "MimeType";
    struct MultivalueHelper
    {
        static void add(const QString &tag, const QString &key, const QString &value, bool expected)
        {
            Values values;
            values["Type"] = "Application";
            values["Name"] = "FooApp";
            values["Exec"] = "true";
            values[key] = value;

            QTest::newRow(qPrintable(QString("multivalue_%1_%2").arg(key).arg(tag)))
                << values << expected;
        }
    };
    foreach (const QString &key, multivalueKeys) {
        MultivalueHelper::add("endSemicolon", key, "foo;", true);
        MultivalueHelper::add("noEndSemicolon", key, "foo", false);
        MultivalueHelper::add("escapedEndSemicolon", key, "foo\\;", false);
        MultivalueHelper::add("endSemicolonWhitespace", key, "foo; ", true);
    }

    struct HeadTailHelper
    {
        static void add(const QString &tag, const QString &head, const QString &tail, bool expected)
        {
            Values values;
            values["Type"] = "Application";
            values["Name"] = "Foo";
            values["Exec"] = "true";
            if (!head.isNull()) {
                values["__head__"] = head;
            }
            if (!tail.isNull()) {
                values["__tail__"] = tail;
            }
            QTest::newRow(qPrintable(QString("headTail_%1").arg(tag))) << values << expected;

        }
    };

    HeadTailHelper::add("badFirstGroup", "[My Group]\nfoo=bar\n[Desktop Entry]", QString(), false);
    HeadTailHelper::add("startsWithSpaces", "  \t\n  \t \n\n[Desktop Entry]", QString(), true);
    HeadTailHelper::add("startsWithComment", "# lorem ipsum\n\n[Desktop Entry]", QString(), true);
    HeadTailHelper::add("startsWithNoGroup", "foo=bar\n\n[Desktop Entry]", QString(), true);
    HeadTailHelper::add("duplicateDesktopEntryGroup", "[Desktop Entry]\nfoo=bar\n\n[Desktop Entry]",
            QString(), false);
    HeadTailHelper::add("duplicateAnyGroup", QString(), "[Foo]\nfoo=bar\n\n[Foo]\nbar=baz", false);
}

void UtMDesktopEntry::isValid()
{
    QEXPECT_FAIL("general_emptyType", "FIXME", Continue);
    QEXPECT_FAIL("general_emptyName", "FIXME", Continue);
    QEXPECT_FAIL("general_appEmptyExec", "FIXME", Continue);
    QEXPECT_FAIL("general_linkEmptyUrl", "FIXME", Continue);

    QFETCH(Values, values);
    QFETCH(bool, expected);

    MDesktopEntry entry(createDesktopEntry(values));
    QCOMPARE(entry.isValid(), expected);
}

void UtMDesktopEntry::contains()
{
    Values values;
    values["foo"] = "bar";

    MDesktopEntry entry(createDesktopEntry(values));
    QVERIFY(entry.contains("Desktop Entry/foo"));
    QVERIFY(entry.contains("Desktop Entry", "foo"));
    QVERIFY(!entry.contains("Desktop Entry/fo"));
    QVERIFY(!entry.contains("Desktop Entry", "fo"));
    QVERIFY(!entry.contains("Desktop Entry/bar"));
    QVERIFY(!entry.contains("Desktop Entry", "bar"));
}

void UtMDesktopEntry::value()
{
    Values values;
    values["foo"] = "bar";

    MDesktopEntry entry(createDesktopEntry(values));
    QCOMPARE(entry.value("Desktop Entry/foo"), QString("bar"));
    QCOMPARE(entry.value("Desktop Entry", "foo"), QString("bar"));
}

void UtMDesktopEntry::duplicateKey()
{
    Values values;
    values["foo"] = "bar";
    values["__tail__"] = "foo=baz";

    MDesktopEntry entry(createDesktopEntry(values));
    QCOMPARE(entry.value("Desktop Entry/foo"), QString("bar"));
}

void UtMDesktopEntry::values_data()
{
    QTest::addColumn<Values>("values");
    QTest::addColumn<QString>("key");
    QTest::addColumn<QString>("expected");

    Values values;
    values["Type"] = "Application";
    values["Version"] = "1.0";
    values["Name"] = "Foo";
    values["GenericName"] = "Bar";
    values["NoDisplay"] = "true";
    values["Comment"] = "Lorem ipsum";
    values["Icon"] = "application/foo";
    values["Hidden"] = "true";
    values["OnlyShowIn"] = "KDE;";
    values["NotShowIn"] = "GNOME;XFCE;";
    values["TryExec"] = "true";
    values["Exec"] = "true";
    values["Path"] = "/tmp";
    values["Terminal"] = "true";
    values["MimeType"] = "text/plain;";
    values["Categories"] = "foo;bar;";
    values["StartupNotify"] = "true";
    values["StartupWMClass"] = "Foo";
    values["URL"] = "http://www.foo.org";
    values["X-Maemo-Service"] = "FooSrv";

    QMapIterator<QString, QString> it(values);
    while (it.hasNext()) {
        it.next();
        QTest::newRow(qPrintable(it.key())) << values << it.key() << it.value();
    }
}

void UtMDesktopEntry::values()
{
    QFETCH(Values, values);
    QFETCH(QString, key);
    QFETCH(QString, expected);

    MDesktopEntry entry(createDesktopEntry(values));

    if (key == "Type") {
        QCOMPARE(entry.type(), expected);
    } else if (key == "Version") {
        QCOMPARE(entry.version(), expected);
    } else if (key == "Name") {
        QCOMPARE(entry.name(), expected);
    } else if (key == "GenericName") {
        QCOMPARE(entry.genericName(), expected);
    } else if (key == "NoDisplay") {
        QCOMPARE(entry.noDisplay() ? QString("true") : QString("false"), expected);
    } else if (key == "Comment") {
        QCOMPARE(entry.comment(), expected);
    } else if (key == "Icon") {
        QCOMPARE(entry.icon(), expected);
    } else if (key == "Hidden") {
        QCOMPARE(entry.hidden() ? QString("true") : QString("false"), expected);
    } else if (key == "OnlyShowIn") {
        QCOMPARE(entry.onlyShowIn(), expected.split(QRegExp("\\s*;\\s*"), QString::SkipEmptyParts));
    } else if (key == "NotShowIn") {
        QCOMPARE(entry.notShowIn(), expected.split(QRegExp("\\s*;\\s*"), QString::SkipEmptyParts));
    } else if (key == "TryExec") {
        QCOMPARE(entry.tryExec(), expected);
    } else if (key == "Exec") {
        QCOMPARE(entry.exec(), expected);
    } else if (key == "Path") {
        QCOMPARE(entry.path(), expected);
    } else if (key == "Terminal") {
        QCOMPARE(entry.terminal() ? QString("true") : QString("false"), expected);
    } else if (key == "MimeType") {
        QCOMPARE(entry.mimeType(), expected.split(QRegExp("\\s*;\\s*"), QString::SkipEmptyParts));
    } else if (key == "Categories") {
        QCOMPARE(entry.categories(), expected.split(QRegExp("\\s*;\\s*"), QString::SkipEmptyParts));
    } else if (key == "StartupNotify") {
        QCOMPARE(entry.startupNotify() ? QString("true") : QString("false"), expected);
    } else if (key == "StartupWMClass") {
        QCOMPARE(entry.startupWMClass(), expected);
    } else if (key == "URL") {
        QCOMPARE(entry.url(), expected);
    } else if (key == "X-Maemo-Service") {
        QCOMPARE(entry.xMaemoService(), expected);
    } else {
        Q_ASSERT_X(false, Q_FUNC_INFO, qPrintable(QString("Invalid key: '%1'").arg(key)));
    }
}

void UtMDesktopEntry::localization_data()
{
    QTest::addColumn<Values>("values");
    QTest::addColumn<QString>("lang");
    QTest::addColumn<QString>("expected");

    struct Helper
    {
        static void add(const QString &tag, const QString &extraValues, const QString &lang,
                const QString &expected)
        {
            Values values;
            values["Type"] = "Application";
            values["Name"] = "Foo";
            values["Exec"] = "true";
            values["__tail__"] = extraValues;

            QTest::newRow(qPrintable(tag)) << values << lang << expected;
        }
    };

    Helper::add("nonLocalized", QString(), QString(), "Foo");
    Helper::add("noLocalizationForLang", "Name[sr]=SrFoo", "en_US", "Foo");

    Helper::add("lcv:lcv", "Name[sr_YU@Latn]=SrFoo", "sr_YU@Latn", "SrFoo");
    Helper::add("lc_:lcv", "Name[sr_YU]=SrFoo", "sr_YU@Latn", "SrFoo");
    Helper::add("l_v:lcv", "Name[sr@Latn]=SrFoo", "sr_YU@Latn", "SrFoo");
    Helper::add("l__:lcv", "Name[sr]=SrFoo", "sr_YU@Latn", "SrFoo");

    Helper::add("lcv:lc_", "Name[sr_YU@Latn]=SrFoo", "sr_YU", "Foo");
    Helper::add("lc_:lc_", "Name[sr_YU]=SrFoo", "sr_YU", "SrFoo");
    Helper::add("l_v:lc_", "Name[sr@Latn]=SrFoo", "sr_YU", "Foo");
    Helper::add("l__:lc_", "Name[sr]=SrFoo", "sr_YU", "SrFoo");

    Helper::add("lcv:l_v", "Name[sr_YU@Latn]=SrFoo", "sr@Latn", "Foo");
    Helper::add("lc_:l_v", "Name[sr_YU]=SrFoo", "sr@Latn", "Foo");
    Helper::add("l_v:l_v", "Name[sr@Latn]=SrFoo", "sr@Latn", "SrFoo");
    Helper::add("l__:l_v", "Name[sr]=SrFoo", "sr@Latn", "SrFoo");

    Helper::add("lcv:l__", "Name[sr_YU@Latn]=SrFoo", "sr", "Foo");
    Helper::add("lc_:l__", "Name[sr_YU]=SrFoo", "sr", "Foo");
    Helper::add("l_v:l__", "Name[sr@Latn]=SrFoo", "sr", "Foo");
    Helper::add("l__:l__", "Name[sr]=SrFoo", "sr", "SrFoo");

    Helper::add("order_lcv",
            "Name[sr]=l__Foo\n"
            "Name[sr@Latn]=l_vFoo\n"
            "Name[sr_YU]=lc_Foo\n"
            "Name[sr_YU@Latn]=lcvFoo\n",
            "sr_YU@Latn", "lcvFoo");
    Helper::add("order_lc_",
            "Name[sr]=l__Foo\n"
            "Name[sr@Latn]=l_vFoo\n"
            "Name[sr_YU]=lc_Foo\n",
            "sr_YU@Latn", "lc_Foo");
    Helper::add("order_l_v",
            "Name[sr]=l__Foo\n"
            "Name[sr@Latn]=l_vFoo\n",
            "sr_YU@Latn", "l_vFoo");

    Helper::add("logical_noTranslation", "X-MeeGo-Logical-Id=LogicalFoo", QString(), "LogicalFoo");
}

void UtMDesktopEntry::localization()
{
    QFETCH(Values, values);
    QFETCH(QString, lang);
    QFETCH(QString, expected);

    if (!lang.isEmpty()) {
        qputenv("LANG", lang.toLocal8Bit());
    }

    MDesktopEntry entry(createDesktopEntry(values));
    QCOMPARE(entry.nameUnlocalized(), QString("Foo"));
    QCOMPARE(entry.name(), expected);
}

QString UtMDesktopEntry::createDesktopEntry(const Values &values)
{
    Q_ASSERT(m_temporaryFile == 0);

    m_temporaryFile = new QTemporaryFile(QDir::temp().filePath("ut_mdesktopentry-%1.desktop")
            .arg(QTest::currentTestFunction()));
    if (!m_temporaryFile->open()) {
        // something horrible must happen to reach this - not sensible to continue
        qFatal("Failed to open temporary file: %s", qPrintable(m_temporaryFile->errorString()));
    }

    QTextStream out(m_temporaryFile);

    if (values.contains("__head__")) {
        out << values.value("__head__") << endl;
    } else {
        out << "[Desktop Entry]" << endl;
    }

    QMapIterator<QString, QString> it(values);
    while (it.hasNext()) {
        it.next();
        if (!it.key().startsWith("__")) {
            out << it.key() << '=' << it.value() << endl;
        }
    }

    if (values.contains("__tail__")) {
        out << values.value("__tail__") << endl;
    }

    return m_temporaryFile->fileName();
}

QTEST_MAIN(Tests::UtMDesktopEntry)

#include "ut_mdesktopentry.moc"
