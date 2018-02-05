#include "PageParserTest.h"
#include <QTest>
#include <PageParser.h>
#include <Qt>

PageParserTest::PageParserTest(QObject *parent) : QObject(parent)
{
}

void PageParserTest::initTestCase()
{
    QDir datasetDir(QString (DATASET_DIR));
    if(!datasetDir.exists()) {
        QFAIL("Please check that <project dir>\"dataset\" directory exist! and SET 'DATASET_DIR' in .pro file.");
    }
}

void PageParserTest::testHtml_1_success()
{
    parse("htmlcxx.sourceforge.net", 4);
}

void PageParserTest::testHtml_2_success()
{
    parse("www.apache.org_licenses_LICENSE-2.0", 0);
}

void PageParserTest::testHtml_3_success()
{
    parse("www.fsf.org", 52);
}

void PageParserTest::testHtml_empty_success()
{
     parse("empty", 0);
}

void PageParserTest::testHtml_no_links_success()
{
    parse("no_links", 0);
}


QString PageParserTest::getFileData(const QString &fileName)
{
    QString result;
    QFile f(QString (DATASET_DIR) + fileName);
    if (!f.exists() || !f.open(QFile::ReadOnly | QFile::Text)) {
        QTest::qFail(QString("File '" + fileName + "' not exist!").toStdString().c_str(),__FILE__, __LINE__ );
        return result;
    }
    QTextStream in(&f);
    result.append(in.readAll());
    f.close();
    return result;
}

void PageParserTest::parse(const QString &fileName, const int expectedLinksCount)
{
    QString html = getFileData(fileName);
    PageParser p(html);
    QStringList urls = p.getUrls();
    QCOMPARE(urls.size(), expectedLinksCount);
    for (auto& url : urls) {
        if (!url.contains("http", Qt::CaseSensitive)) {
            QFAIL("Bad url!!");
        }
    }
}
