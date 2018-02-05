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
QQueue<PageLoader*> jobs;
bool bPaused = false;
static int count = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->button_start, SIGNAL (released()), this, SLOT (handleButton()));

    mainLoopTimer = new QTimer(this);
    connect(mainLoopTimer, &QTimer::timeout, [this]
    {

        QThreadPool *threadPool = QThreadPool::globalInstance();

        QMutexLocker locker(&queueMutex);
        if (jobs.isEmpty()) {
            return;
        }
        PageLoader* worker = jobs.dequeue();
        QObject::connect(worker, SIGNAL(finished(QStringList)),
                         this, SLOT(threadFinished(QStringList)),
                         Qt::QueuedConnection);
        threadPool->start(worker);
        ++count;
        if (count > 5) {
            qDebug() << "Stop LOOP!!";

            mainLoopTimer->stop();
            return;
        }

        qDebug()<< QThreadPool::globalInstance()->activeThreadCount();
    } );

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::threadFinished(QStringList urls) {
    QMutexLocker locker(&queueMutex);
    qDebug() <<"GOT URLS in main thread:";

    for (auto& it : urls) {
        qDebug() << it;
        jobs.enqueue(new PageLoader(it));
    }

}


void MainWindow::handleButton() {
    qDebug() <<"start button pressed!\n";

    PageLoader* firstItem = new PageLoader("http://www.fsf.org");
    firstItem->run();

    jobs.enqueue(firstItem);

    mainLoopTimer->start(0);


}
