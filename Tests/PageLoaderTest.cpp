#include "PageLoaderTest.h"
#include <PageLoader.h>
#include <QTest>
#include <QSignalSpy>

PageLoaderTest::PageLoaderTest(QObject *parent) : QObject(parent)
{

}


void PageLoaderTest::initTestCase()
{
}

void PageLoaderTest::test_1_not_found()
{
    PageLoader p("http://htmlcxx.sourceforge.net", "XXX", false, 0);
    QSignalSpy spy(&p, SIGNAL(pageLoaded(int, PageLoader::Status, QString, QStringList, int)));

    // trigger emission of the signal
    p.start();

    QStringList urls;
    verifyResult(spy, PageLoader::Status::NOT_FOUND, urls);
}

void PageLoaderTest::test_2_found()
{
    PageLoader p("http://htmlcxx.sourceforge.net", "Example", false, 0);
    QSignalSpy spy(&p, SIGNAL(pageLoaded(int, PageLoader::Status, QString, QStringList, int)));

    p.start();

    QStringList urls;
    verifyResult(spy, PageLoader::Status::FOUND, urls);
}


void PageLoaderTest::test_3_bad_type()
{
    PageLoader p("https://download.mozilla.org/?product=firefox-latest-ssl&amp;os=osx&amp;lang=en-US", "XXX", false, 0);
    QSignalSpy spy(&p, SIGNAL(pageLoaded(int, PageLoader::Status, QString, QStringList, int)));

    p.start();

    QStringList urls;
    verifyResult(spy, PageLoader::Status::HTTP_NO_TEXT, urls);
}

void PageLoaderTest::test_4_bad_host()
{
    PageLoader p("http://unknown_host_name", "XXX", false, 0);
    QSignalSpy spy(&p, SIGNAL(pageLoaded(int, PageLoader::Status, QString, QStringList, int)));

    p.start();

    QStringList urls;
    verifyResult(spy, PageLoader::Status::HTTP_ERROR, urls);
}

void PageLoaderTest::verifyResult(QSignalSpy &spy, const PageLoader::Status& a_status, QStringList& a_urls)
{
    spy.wait(10000);

    QCOMPARE(spy.count(), 1); // make sure the signal was emitted exactly one time

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).type() == QVariant::Int);
    QVariant status_var = arguments.at(1);
    QVERIFY(true == status_var.canConvert<PageLoader::Status>());
    QVERIFY(arguments.at(2).type() == QVariant::String);
    QVERIFY(arguments.at(3).type() == QVariant::StringList);
    QVERIFY(arguments.at(4).type() == QVariant::Int);


    // Check
    QVERIFY(qvariant_cast<PageLoader::Status>(status_var) == a_status); //1
    QVERIFY(arguments.at(2).toString().size() != 0); //2
    a_urls = arguments.at(3).toStringList();
}

