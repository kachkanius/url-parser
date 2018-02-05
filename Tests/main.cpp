#include <QApplication>

#include <QApplication>
#include <QTest>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "PageParserTest.h"

using namespace std;


int main(int argc, char *argv[])
{
//    freopen("testing.log", "w", stdout);
    QApplication a(argc, argv);
    QTest::qExec(new PageParserTest, argc, argv);
    return 0;
}
