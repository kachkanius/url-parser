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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->button_start, SIGNAL (released()), this, SLOT (startButtonPressed()));
    connect(ui->button_stop, SIGNAL (released()), this, SLOT (stopButtonPressed()));

    connect(&m_manager, SIGNAL (addItem(QString, size_t)), this, SLOT (addItem(QString, size_t)));
    connect(&m_manager, SIGNAL (updateItem(size_t, PageLoader::Status)), this, SLOT (updateItem(size_t, PageLoader::Status)));






}


MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::doHeadJob()
//{
//    if (jobs->isEmpty()) {
//        return;
//    }
//    PageLoader* worker = jobs->dequeue();
//    QObject::connect(worker, SIGNAL(finished(QStringList, int)),
//                     this, SLOT(threadFinished(QStringList, int)),
//                     Qt::QueuedConnection);
//    QThreadPool::globalInstance()->start(worker);
//}


//void MainWindow::threadFinished(QStringList urls, int depth) {
//    QMutexLocker locker(&queueMutex);
//    qDebug() <<"GOT URLS in main thread:";

//    if (grapth.size() <= depth + 1) {
//        grapth.push_back(QQueue<PageLoader*>());
//    }

//    for (auto& it : urls) {
//        qDebug() << it;
//        grapth[depth + 1].enqueue(new PageLoader(it, depth + 1));
//        ++count;
//        if (count > 50) {
//            qDebug() << "Stop LOOP!!";
//            QThreadPool::globalInstance()->clear();
//            return;
//        }
//    }

//    if (jobs->empty() && grapth.size() > depth) {
//        jobs = &grapth[depth + 1];
//    }

//    doHeadJob();

//}


void MainWindow::startButtonPressed() {
//    qDebug() <<"start button pressed!\n";
//    QThreadPool::globalInstance()->setMaxThreadCount(2);


//    PageLoader* firstItem = new PageLoader("http://htmlcxx.sourceforge.net", 0);
//    QQueue<PageLoader*> zero;
//    zero.enqueue(firstItem);
//    grapth.push_back(zero);
//    jobs = &grapth[0];

//    doHeadJob();
    ui->text_browser->clear();

    m_manager.setMaxScannedLinks(50);
    m_manager.setMaxThreadsCount(8);
    m_manager.start("http://htmlcxx.sourceforge.net", "Hello");

}

void MainWindow::stopButtonPressed()
{
    m_manager.stop();
}

void MainWindow::updateItem(size_t id, PageLoader::Status status)
{
    QMutexLocker locker(&m_textMutex);
    qDebug() <<  "Update item# " + QString::number(id) << " status :" << (int)status;
}

void MainWindow::addItem(QString url, size_t id)
{
    QMutexLocker locker(&m_textMutex);
    QString item(url + " " +  QString::number(id) + " LOADING ");
    ui->text_browser->append(item);

}
