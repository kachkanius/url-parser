#ifndef ENGINETESTS_H
#define ENGINETESTS_H

#include <QObject>

class PageParserTest : public QObject
{
    Q_OBJECT
public:
    explicit PageParserTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void testHtml_1_success();
    void testHtml_2_success();
    void testHtml_3_success();
    void testHtml_empty_success();
    void testHtml_no_links_success();
private:
    QString getFileData(const QString& fileName);
    void parse(const QString& fileName, const int expectedLinksCount);

};

#endif // ENGINETESTS_H
