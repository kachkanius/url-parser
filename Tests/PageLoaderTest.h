#ifndef PAGELOADERTEST_H
#define PAGELOADERTEST_H

#include <QObject>
#include <QSignalSpy>
#include <PageLoader.h>

class PageLoaderTest : public QObject
{
    Q_OBJECT
public:
    explicit PageLoaderTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void test_1_not_found();
    void test_2_found();
    void test_3_bad_type();
    void test_4_bad_host();

private:
    void verifyResult(QSignalSpy& spy, const PageLoader::Status &a_status, QStringList &a_urls);

};

#endif // PAGELOADERTEST_H
