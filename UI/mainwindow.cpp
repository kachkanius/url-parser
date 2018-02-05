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
#include <QTimer>

#include <PageParser.h>
#include <PageLoader.h>

#include <QMutex>
#include <QMutexLocker>

QMutex queueMutex;
QQueue<PageLoader*>* jobs;
bool bPaused = false;
QVector<QQueue<PageLoader*>> grapth;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    count(0)
{
    ui->setupUi(this);
    connect(ui->button_start, SIGNAL (released()), this, SLOT (handleButton()));

    mainLoopTimer = new QTimer(this);
    connect(mainLoopTimer, &QTimer::timeout, [this]
    {

        QThreadPool *threadPool = QThreadPool::globalInstance();

        QMutexLocker locker(&queueMutex);
        if (jobs->isEmpty()) {
            return;
        }
        PageLoader* worker = jobs->dequeue();
        QObject::connect(worker, SIGNAL(finished(QStringList, int)),
                         this, SLOT(threadFinished(QStringList, int)),
                         Qt::QueuedConnection);
        threadPool->start(worker);
//        ++count;


//        qDebug()<< QThreadPool::globalInstance()->activeThreadCount();
    } );

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::threadFinished(QStringList urls, int depth) {
    QMutexLocker locker(&queueMutex);
    qDebug() <<"GOT URLS in main thread:";

    if (grapth.size() <= depth + 1) {
        grapth.push_back(QQueue<PageLoader*>());
    }

    for (auto& it : urls) {
        qDebug() << it;
        grapth[depth + 1].enqueue(new PageLoader(it, depth + 1));
        ++count;
        if (count > 500) {
            qDebug() << "Stop LOOP!!";

            mainLoopTimer->stop();
        }
    }

    if (jobs->empty() && grapth.size() > depth) {
        jobs = &grapth[depth + 1];
    }

}


void MainWindow::handleButton() {
    qDebug() <<"start button pressed!\n";
    QThreadPool::globalInstance()->setMaxThreadCount(2);

    PageLoader* firstItem = new PageLoader("http://htmlcxx.sourceforge.net", 0);
    QQueue<PageLoader*> zero;
    zero.enqueue(firstItem);
    grapth.push_back(zero);
    jobs = &grapth[0];

    mainLoopTimer->start(0);
}
