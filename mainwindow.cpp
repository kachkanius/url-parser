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


class HelloWorldTask : public QRunnable
{
    void run()
    {
        qDebug() << "Hello world from thread" << QThread::currentThreadId();
        QEventLoop loop;
        QNetworkAccessManager nam;

    //    QNetworkRequest req(QUrl("http://htmlcxx.sourceforge.net/"));
    //    req.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:10.0) Gecko/20100101 Firefox/10.0");

    //      QNetworkRequest req(QUrl("http://htmlcxx.sourceforge.net"));
          QNetworkRequest req(QUrl("http://www.fsf.org"));


        req.setRawHeader("User-Agent", "Mozilla/5.0 (Android; Mobile; rv:40.0) Gecko/40.0 Firefox/40.0");
        QNetworkReply *reply = nam.get(req);
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray buffer = reply->readAll();
//        qDebug() << reply->rawHeader("Location");
        qDebug() << "HTTP status: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//        ui->text_browser->setText(QString(buffer));

        qDebug() << buffer;

        QString file_name = "result_" + QString::number(*(u_int64_t*)QThread::currentThreadId());;
        qDebug() << "File: "<<file_name;
        QFile file(file_name);
        file.open(QIODevice::WriteOnly);
        file.write(buffer);
        file.close();
    }
};



void MainWindow::handleButton() {
    qDebug() <<"start button pressed!\n";
    QThreadPool::globalInstance()->setMaxThreadCount(10);

    HelloWorldTask *hello = new HelloWorldTask();

    // QThreadPool takes ownership and deletes 'hello' automatically
    QThreadPool::globalInstance()->start(hello);

    HelloWorldTask *hello2 = new HelloWorldTask();

    // QThreadPool takes ownership and deletes 'hello' automatically
QThreadPool::globalInstance()->start(hello2);

}
