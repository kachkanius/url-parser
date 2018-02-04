#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QRunnable>
#include <QThread>

#include <QThreadPool>
#include <QQueue>

#include <PageParser.h>
#include <PageLoader.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->button_start, SIGNAL (released()), this, SLOT (handleButton()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

//"http://htmlcxx.sourceforge.net"
QQueue<PageLoader*> jobs;


void MainWindow::threadFinished(QStringList urls) {
    qDebug() <<"GOT URLS in main thread:";

    for (auto& it : urls) {
        qDebug() << it;
    }
}

void MainWindow::handleButton() {
    qDebug() <<"start button pressed!\n";

    //    QFile file("result_139988983199488");
    //    file.open(QIODevice::ReadOnly);
    //    qint64 s = file.size();
    //    QByteArray text = file.read(s);
    //    file.close();


    //    QString res(text);
    //    PageParser p(res);
    //    PageParser::URLS re =  p.getUrls();

    //    for (auto& it : re) {
    //        qDebug() << it;
    //    }

    //        ui->text_browser->setText(QString(buffer));
    qDebug()<< QThreadPool::globalInstance()->activeThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(2);


    PageLoader* start = new PageLoader("http://htmlcxx.sourceforge.net");
    jobs.enqueue(start);

    QThreadPool *threadPool = QThreadPool::globalInstance();
    while (!jobs.empty()) {
        PageLoader* worker = jobs.dequeue();
        QObject::connect(worker, SIGNAL(finished(QStringList)),
                         this, SLOT(threadFinished(QStringList)),
                         Qt::QueuedConnection);
        threadPool->start(worker);
    }

    qDebug()<< QThreadPool::globalInstance()->activeThreadCount();


}
