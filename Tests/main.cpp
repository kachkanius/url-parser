#include <QApplication>

#include <QApplication>
#include <QTest>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "PageParserTest.h"
#include "PageLoaderTest.h"
using namespace std;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTest::qExec(new PageParserTest, argc, argv);
    QTest::qExec(new PageLoaderTest, argc, argv);

    return 0;

}
