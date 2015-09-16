#include <dconf/dconf.h>

#include <QTest>


#include "mdconfgroup.h"

#include "testbase.h"

#include <QColor>
#include <QDebug>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QStringList>
#include <QVariant>
#include <QVector>

#include <QSignalSpy>

namespace Tests {

class UtMDConfGroup : public TestBase
{
    Q_OBJECT
public:
    UtMDConfGroup()
        : m_client(0)
    {
    }

    void propertyData();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void asynchronous_data() { propertyData(); }
    void asynchronous();
    void synchronous_data() { propertyData(); }
    void synchronous();
    void value_data() { propertyData(); }
    void value();
    void scopes();
    void pathChangeInResolve();

private:
    DConfClient *m_client;
};


class TestConfGroup : public MDConfGroup
{
    Q_OBJECT
    Q_PROPERTY(bool              booleanProperty       READ booleanProperty       WRITE setBooleanProperty       NOTIFY booleanPropertyChanged)
    Q_PROPERTY(char              characterProperty     READ characterProperty     WRITE setCharacterProperty     NOTIFY characterPropertyChanged)
    Q_PROPERTY(qint16            int16Property         READ int16Property         WRITE setInt16Property         NOTIFY int16PropertyChanged)
    Q_PROPERTY(quint16           uint16Property        READ uint16Property        WRITE setUint16Property        NOTIFY uint16PropertyChanged)
    Q_PROPERTY(qint32            int32Property         READ int32Property         WRITE setInt32Property         NOTIFY int32PropertyChanged)
    Q_PROPERTY(quint32           uint32Property        READ uint32Property        WRITE setUint32Property        NOTIFY uint32PropertyChanged)
    Q_PROPERTY(qint64            int64Property         READ int64Property         WRITE setInt64Property         NOTIFY int64PropertyChanged)
    Q_PROPERTY(quint64           uint64Property        READ uint64Property        WRITE setUint64Property        NOTIFY uint64PropertyChanged)
    Q_PROPERTY(float             floatProperty         READ floatProperty         WRITE setFloatProperty         NOTIFY floatPropertyChanged)
    Q_PROPERTY(double            doubleProperty        READ doubleProperty        WRITE setDoubleProperty        NOTIFY doublePropertyChanged)
    Q_PROPERTY(QString           stringProperty        READ stringProperty        WRITE setStringProperty        NOTIFY stringPropertyChanged)
    Q_PROPERTY(QByteArray        bytearrayProperty     READ bytearrayProperty     WRITE setBytearrayProperty     NOTIFY bytearrayPropertyChanged)
    Q_PROPERTY(QStringList       stringlistProperty    READ stringlistProperty    WRITE setStringlistProperty    NOTIFY stringlistPropertyChanged)
    Q_PROPERTY(QVariantList      variantlistProperty   READ variantlistProperty   WRITE setVariantlistProperty   NOTIFY variantlistPropertyChanged)
    Q_PROPERTY(QPoint            pointProperty         READ pointProperty         WRITE setPointProperty         NOTIFY pointPropertyChanged)
    Q_PROPERTY(QPointF           pointfProperty        READ pointfProperty        WRITE setPointfProperty        NOTIFY pointfPropertyChanged)
    Q_PROPERTY(QSize             sizeProperty          READ sizeProperty          WRITE setSizeProperty          NOTIFY sizePropertyChanged)
    Q_PROPERTY(QSizeF            sizefProperty         READ sizefProperty         WRITE setSizefProperty         NOTIFY sizefPropertyChanged)
    Q_PROPERTY(QRect             rectProperty          READ rectProperty          WRITE setRectProperty          NOTIFY rectPropertyChanged)
    Q_PROPERTY(QRectF            rectfProperty         READ rectfProperty         WRITE setRectfProperty         NOTIFY rectfPropertyChanged)
    Q_PROPERTY(QList<QByteArray> bytearraylistProperty READ bytearraylistProperty WRITE setBytearraylistProperty NOTIFY bytearraylistPropertyChanged)
    Q_PROPERTY(QColor            colorProperty         READ colorProperty         WRITE setColorProperty         NOTIFY colorPropertyChanged)
    Q_PROPERTY(QVariantMap       variantmapProperty    READ variantmapProperty    WRITE setVariantmapProperty    NOTIFY variantmapPropertyChanged)

public:
    TestConfGroup(QObject *parent = 0)
        : MDConfGroup(parent, BindProperties)
        , m_booleanProperty(false)
        , m_characterProperty(0)
        , m_int16Property(0)
        , m_uint16Property(0)
        , m_int32Property(0)
        , m_uint32Property(0)
        , m_int64Property(0)
        , m_uint64Property(0)
        , m_floatProperty(0)
        , m_doubleProperty(0)
    {
        resolveMetaObject(staticMetaObject.propertyOffset());
    }

    bool booleanProperty() { return m_booleanProperty; }
    char characterProperty() { return m_characterProperty; }
    qint16 int16Property() { return m_int16Property; }
    quint16 uint16Property() { return m_uint16Property; }
    qint32 int32Property() { return m_int32Property; }
    quint32 uint32Property() { return m_uint32Property; }
    qint64 int64Property() { return m_int64Property; }
    quint64 uint64Property() { return m_uint64Property; }
    float floatProperty() { return m_floatProperty; }
    double doubleProperty() { return m_doubleProperty; }
    QString stringProperty() { return m_stringProperty; }
    QByteArray bytearrayProperty() { return m_bytearrayProperty; }
    QStringList stringlistProperty() { return m_stringlistProperty; }
    QVariantList variantlistProperty() { return m_variantlistProperty; }
    QPoint pointProperty() { return m_pointProperty; }
    QPointF pointfProperty() { return m_pointfProperty; }
    QSize sizeProperty() { return m_sizeProperty; }
    QSizeF sizefProperty() { return m_sizefProperty; }
    QRect rectProperty() { return m_rectProperty; }
    QRectF rectfProperty() { return m_rectfProperty; }
    QList<QByteArray> bytearraylistProperty() { return m_bytearraylistProperty; }
    QColor colorProperty() { return m_colorProperty; }
    QVariantMap variantmapProperty() { return m_variantmapProperty; }

    void setBooleanProperty(bool booleanProperty) { m_booleanProperty = booleanProperty; emit booleanPropertyChanged(); }
    void setCharacterProperty(char characterProperty) { m_characterProperty = characterProperty; emit characterPropertyChanged(); }
    void setInt16Property(qint16 int16Property) { m_int16Property = int16Property; emit int16PropertyChanged(); }
    void setUint16Property(quint16 uint16Property) { m_uint16Property = uint16Property; emit uint16PropertyChanged(); }
    void setInt32Property(qint32 int32Property ) { m_int32Property = int32Property; emit int32PropertyChanged(); }
    void setUint32Property(quint32 uint32Property) { m_uint32Property = uint32Property; emit uint32PropertyChanged(); }
    void setInt64Property(qint64 int64Property) { m_int64Property = int64Property; emit int64PropertyChanged(); }
    void setUint64Property(quint64 uint64Property) { m_uint64Property = uint64Property; emit uint64PropertyChanged(); }
    void setFloatProperty(float floatProperty) { m_floatProperty = floatProperty; emit floatPropertyChanged(); }
    void setDoubleProperty(double doubleProperty) { m_doubleProperty = doubleProperty; emit doublePropertyChanged(); }
    void setStringProperty(QString stringProperty) { m_stringProperty = stringProperty; emit stringPropertyChanged(); }
    void setBytearrayProperty(QByteArray bytearrayProperty) { m_bytearrayProperty = bytearrayProperty; emit bytearrayPropertyChanged(); }
    void setStringlistProperty(QStringList stringlistProperty) { m_stringlistProperty = stringlistProperty; emit stringlistPropertyChanged(); }
    void setVariantlistProperty(QVariantList variantlistProperty) { m_variantlistProperty = variantlistProperty; emit variantlistPropertyChanged(); }
    void setPointProperty(QPoint pointProperty) { m_pointProperty = pointProperty; emit pointPropertyChanged(); }
    void setPointfProperty(QPointF pointfProperty) { m_pointfProperty = pointfProperty; emit pointfPropertyChanged(); }
    void setSizeProperty(QSize sizeProperty) { m_sizeProperty = sizeProperty; emit sizePropertyChanged(); }
    void setSizefProperty(QSizeF sizefProperty) { m_sizefProperty = sizefProperty; emit sizefPropertyChanged(); }
    void setRectProperty(QRect rectProperty) { m_rectProperty = rectProperty; emit rectPropertyChanged(); }
    void setRectfProperty(QRectF rectfProperty) { m_rectfProperty = rectfProperty; emit rectfPropertyChanged(); }
    void setBytearraylistProperty(QList<QByteArray> bytearraylistProperty) { m_bytearraylistProperty = bytearraylistProperty; emit bytearraylistPropertyChanged(); }
    void setColorProperty(QColor colorProperty) { m_colorProperty = colorProperty; emit colorPropertyChanged(); }
    void setVariantmapProperty(QVariantMap variantmapProperty) { m_variantmapProperty = variantmapProperty; emit variantmapPropertyChanged(); }

    using MDConfGroup::resolveMetaObject;

public slots:
    void updatePath();

signals:
    void booleanPropertyChanged();
    void characterPropertyChanged();
    void int16PropertyChanged();
    void uint16PropertyChanged();
    void int32PropertyChanged();
    void uint32PropertyChanged();
    void int64PropertyChanged();
    void uint64PropertyChanged();
    void floatPropertyChanged();
    void doublePropertyChanged();
    void stringPropertyChanged();
    void bytearrayPropertyChanged();
    void stringlistPropertyChanged();
    void variantlistPropertyChanged();
    void pointPropertyChanged();
    void pointfPropertyChanged();
    void sizePropertyChanged();
    void sizefPropertyChanged();
    void rectPropertyChanged();
    void rectfPropertyChanged();
    void bytearraylistPropertyChanged();
    void colorPropertyChanged();
    void variantmapPropertyChanged();

private:
    bool m_booleanProperty;
    char m_characterProperty;
    qint16 m_int16Property;
    quint16 m_uint16Property;
    qint32 m_int32Property;
    quint32 m_uint32Property;
    qint64 m_int64Property;
    quint64 m_uint64Property;
    float m_floatProperty;
    double m_doubleProperty;
    QString m_stringProperty;
    QByteArray m_bytearrayProperty;
    QStringList m_stringlistProperty;
    QVariantList m_variantlistProperty;
    QPoint m_pointProperty;
    QPointF m_pointfProperty;
    QSize m_sizeProperty;
    QSizeF m_sizefProperty;
    QRect m_rectProperty;
    QRectF m_rectfProperty;
    QList<QByteArray> m_bytearraylistProperty;
    QColor m_colorProperty;
    QVariantMap m_variantmapProperty;
};

void TestConfGroup::updatePath()
{
    TestConfGroup *parent = qobject_cast<TestConfGroup *>(sender());
    if (!parent) {
        qWarning() << "updatePath was not connected to a TestConfGroup instance";
        return;
    }

    setPath(parent->stringProperty());
}

}

using namespace Tests;

void UtMDConfGroup::initTestCase()
{
    m_client = dconf_client_new();
}

void UtMDConfGroup::cleanupTestCase()
{
    dconf_client_write_sync(m_client, "/mlite-tests/ut_mdconfgroup/scopes/", 0, 0, 0, 0);
    g_object_unref(m_client);
    m_client = 0;
}

void UtMDConfGroup::init()
{
    dconf_client_write_sync(m_client, "/mlite-tests/ut_mdconfgroup/scopes/", 0, 0, 0, 0);
}

void UtMDConfGroup::propertyData()
{
    QTest::addColumn<QString>("key");
    QTest::addColumn<QVariant>("value1");
    QTest::addColumn<QVariant>("value2");

    QTest::newRow("booleanProperty")
            << "booleanProperty"
            << QVariant::fromValue(true)
            << QVariant::fromValue(false);
    QTest::newRow("characterProperty")
            << "characterProperty"
            << QVariant::fromValue('c')
            << QVariant::fromValue('k');
    QTest::newRow("int16Property")
            << "int16Property"
            << QVariant::fromValue(3432)
            << QVariant::fromValue(4313);
    QTest::newRow("uint16Property")
            << "uint16Property"
            << QVariant::fromValue(32421)
            << QVariant::fromValue(7655);
    QTest::newRow("int32Property")
            << "int32Property"
            << QVariant::fromValue(4454333)
            << QVariant::fromValue(-5434532);
    QTest::newRow("uint32Property")
            << "uint32Property"
            << QVariant::fromValue(545332)
            << QVariant::fromValue(54543545);
    QTest::newRow("int64Property")
            << "int64Property"
            << QVariant::fromValue(4544545LL << 32)
            << QVariant::fromValue(54545LL << 32);
    QTest::newRow("uint64Property")
            << "uint64Property"
            << QVariant::fromValue(4543553LL << 32)
            << QVariant::fromValue(454533LL << 32);
    QTest::newRow("floatProperty")
            << "floatProperty"
            << QVariant::fromValue(454.3f)
            << QVariant::fromValue(5431.1f);
    QTest::newRow("doubleProperty")
            << "doubleProperty"
            << QVariant::fromValue(4545.0)
            << QVariant::fromValue(43.32);
    QTest::newRow("stringProperty")
            << "stringProperty"
            << QVariant::fromValue(QStringLiteral("hello"))
            << QVariant::fromValue(QStringLiteral("goodbye"));
    QTest::newRow("bytearrayProperty")
            << "bytearrayProperty"
            << QVariant::fromValue(QByteArray("somedata"))
            << QVariant::fromValue(QByteArray("someotherdata"));
    QTest::newRow("stringlistProperty")
            << "stringlistProperty"
            << QVariant::fromValue(QStringList() << QStringLiteral("one") << QStringLiteral("two") << QStringLiteral("three"))
            << QVariant::fromValue(QStringList() << QStringLiteral("a") << QStringLiteral("b"));
    QTest::newRow("variantlistProperty")
            << "variantlistProperty"
            << QVariant::fromValue(QVariantList() << 5 << QStringLiteral("a string"))
            << QVariant::fromValue(QVariantList() << QStringLiteral("one") << QStringLiteral("two") << 3 << 4.0);
    QTest::newRow("pointProperty")
            << "pointProperty"
            << QVariant::fromValue(QPoint(4, 6))
            << QVariant::fromValue(QPoint(54, 214));
    QTest::newRow("sizeProperty")
            << "sizeProperty"
            << QVariant::fromValue(QSize(4, 2))
            << QVariant::fromValue(QSize(320, 240));
    QTest::newRow("sizefProperty")
            << "sizefProperty"
            << QVariant::fromValue(QSizeF(43.2, 42.3))
            << QVariant::fromValue(QSizeF(4.5, 9.9991));
    QTest::newRow("rectProperty")
            << "rectProperty"
            << QVariant::fromValue(QRect(2, 3, 56, 67))
            << QVariant::fromValue(QRect(9, 5, 2, 1));
    QTest::newRow("rectfProperty")
            << "rectfProperty"
            << QVariant::fromValue(QRectF(34.3, 342.1, 153.2, 12.22))
            << QVariant::fromValue(QRectF(12.3, 205.3, 98, 32));
    QTest::newRow("colorProperty")
            << "colorProperty"
            << QVariant::fromValue(QColor(Qt::red))
            << QVariant::fromValue(QColor(Qt::blue));

    QVariantMap map1;
    map1.insert(QStringLiteral("propA"), QStringLiteral("one"));
    map1.insert(QStringLiteral("propB"), 2);
    map1.insert(QStringLiteral("propC"), 0.4);

    QVariantMap map2;
    map2.insert(QStringLiteral("prop1"), true);
    map2.insert(QStringLiteral("prop2"), QByteArray("seven"));

    QTest::newRow("variantmapProperty")
            << "variantmapProperty"
            << QVariant::fromValue(map1)
            << QVariant::fromValue(map2);
}

void UtMDConfGroup::asynchronous()
{
    QFETCH(QString, key);
    QFETCH(QVariant, value1);
    QFETCH(QVariant, value2);

    const QByteArray property = key.toUtf8();

    TestConfGroup group1;
    TestConfGroup group2;

    group1.setObjectName("group1");
    group2.setObjectName("group2");

    QSignalSpy propertySpy(&group2, "2" + property + "Changed()");

    group1.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope1/nested/properties"));
    group2.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope1/nested/properties"));

    QCOMPARE(group1.isSynchronous(), false);
    QCOMPARE(group2.isSynchronous(), false);

    QVariant initialValue = group1.property(property);
    QCOMPARE(group2.property(property), initialValue);

    group1.setProperty(property, value1);
    QCOMPARE(group1.property(property), value1);

    QCOMPARE(group2.property(property), initialValue);

    QTRY_COMPARE(propertySpy.count(), 1);

    group1.setProperty(property, value2);
    QCOMPARE(group1.property(property), value2);

    QCOMPARE(group2.property(property), value1);

    QTRY_COMPARE(propertySpy.count(), 2);
    QCOMPARE(group2.property(property), value2);
}

void UtMDConfGroup::synchronous()
{
    QFETCH(QString, key);
    QFETCH(QVariant, value1);
    QFETCH(QVariant, value2);

    const QByteArray property = key.toUtf8();

    TestConfGroup group1;
    TestConfGroup group2;

    QSignalSpy propertySpy(&group2, "2" + property + "Changed()");

    group1.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope1/nested/properties"));
    group2.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope1/nested/properties"));

    QCOMPARE(group1.isSynchronous(), false);
    QCOMPARE(group2.isSynchronous(), false);

    group1.setSynchronous(true);
    group2.setSynchronous(true);

    QCOMPARE(group1.isSynchronous(), true);
    QCOMPARE(group2.isSynchronous(), true);

    QVariant initialValue = group1.property(property);
    QCOMPARE(group2.property(property), initialValue);

    group1.setProperty(property, value1);
    QCOMPARE(group1.property(property), value1);

    QCOMPARE(group2.property(property), initialValue);

    QTRY_COMPARE(propertySpy.count(), 1);
    QCOMPARE(group2.property(property), value1);

    group1.setProperty(property, value2);
    QCOMPARE(group1.property(property), value2);

    QCOMPARE(group2.property(property), value1);

    QTRY_COMPARE(propertySpy.count(), 2);
    QCOMPARE(group2.property(property), value2);
}

void UtMDConfGroup::value()
{
    QFETCH(QString, key);
    QFETCH(QVariant, value1);
    QFETCH(QVariant, value2);

    const QVariant defaultValue = QStringLiteral("default");

    MDConfGroup group1;
    MDConfGroup group2(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope1/nested/properties"));

    group1.setObjectName("group1");
    group2.setObjectName("group2");

    QSignalSpy propertySpy(&group2, SIGNAL(valueChanged(QString)));

    group1.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope1/nested/properties"));

    QCOMPARE(group1.isSynchronous(), false);
    QCOMPARE(group2.isSynchronous(), false);

    QCOMPARE(group1.value(key, defaultValue), defaultValue);
    QCOMPARE(group2.value(key), QVariant());

    group1.setValue(key, value1);
    QCOMPARE(group1.value(key, defaultValue, value1.userType()), value1);

    QTRY_COMPARE(propertySpy.count(), 1);
    QCOMPARE(propertySpy.last().value(0).toString(), key);

    QCOMPARE(group2.value(key, defaultValue, value1.userType()), value1);

    group1.setValue(key, value2);
    QCOMPARE(group1.value(key, defaultValue, value2.userType()), value2);

    QTRY_COMPARE(propertySpy.count(), 2);
    QCOMPARE(propertySpy.last().value(0).toString(), key);

    QCOMPARE(group2.value(key, defaultValue, value2.userType()), value2);
}

void UtMDConfGroup::scopes()
{
    TestConfGroup scope1;
    scope1.setObjectName("scope1");
    scope1.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope1"));

    TestConfGroup scope2;
    scope2.setObjectName("scope2");
    scope2.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes"));

    TestConfGroup nestedScope;
    nestedScope.setObjectName("nestedScope");
    nestedScope.setPath(QStringLiteral("scope1/nested"));
    nestedScope.setScope(&scope2);

    TestConfGroup group1;
    group1.setObjectName("group1");
    group1.setScope(&scope1);
    group1.setPath(QStringLiteral("nested/properties"));

    TestConfGroup group2;
    group2.setObjectName("group2");
    group2.setPath(QStringLiteral("properties"));
    group2.setScope(&nestedScope);

    group1.setStringProperty(QStringLiteral("scope1"));
    QCOMPARE(group1.stringProperty(), QStringLiteral("scope1"));
    QCOMPARE(group2.stringProperty(), QString());
    group1.sync();
    QTRY_COMPARE(group2.stringProperty(), QStringLiteral("scope1"));

    nestedScope.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope2/nested"));
    QCOMPARE(group2.stringProperty(), QStringLiteral("scope1")); // DConf has no value so retain the previous value

    group2.setStringProperty(QStringLiteral("scope2: absolute"));
    QCOMPARE(group2.stringProperty(), QStringLiteral("scope2: absolute"));
    group2.sync();
    QCOMPARE(group1.stringProperty(), QStringLiteral("scope1"));

    nestedScope.setScope(0);
    QCOMPARE(group2.stringProperty(), QStringLiteral("scope2: absolute"));

    group2.setStringProperty(QStringLiteral("scope2: no parent"));
    QCOMPARE(group2.stringProperty(), QStringLiteral("scope2: no parent"));
    group2.sync();
    QCOMPARE(group1.stringProperty(), QStringLiteral("scope1"));

    scope1.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope2"));
    QCOMPARE(group1.stringProperty(), QStringLiteral("scope2: no parent"));

    nestedScope.setPath(QString());
    group2.setStringProperty(QStringLiteral("no scope"));
    QCOMPARE(group2.stringProperty(), QStringLiteral("no scope"));
    group2.sync();
    QCOMPARE(group1.stringProperty(), QStringLiteral("scope2: no parent"));

    nestedScope.setPath(QStringLiteral("nested"));
    QCOMPARE(group2.stringProperty(), QStringLiteral("no scope"));

    nestedScope.setScope(&scope1);
    QCOMPARE(group2.stringProperty(), QStringLiteral("scope2: no parent"));
}

void UtMDConfGroup::pathChangeInResolve()
{
    {
        TestConfGroup group;
        group.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope2"));
        group.setStringProperty(QStringLiteral("nested/properties"));
        group.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope2/nested/properties"));
        group.setStringProperty(QStringLiteral("nested-value"));
    }

    TestConfGroup group;

    TestConfGroup nestedScope;
    nestedScope.setScope(&group);
    connect(&group, SIGNAL(stringPropertyChanged()), &nestedScope, SLOT(updatePath()));

    group.setPath(QStringLiteral("/mlite-tests/ut_mdconfgroup/scopes/scope2"));

    QCOMPARE(nestedScope.stringProperty(), QStringLiteral("nested-value"));
}

QTEST_MAIN(Tests::UtMDConfGroup)

#include "ut_mdconfgroup.moc"
