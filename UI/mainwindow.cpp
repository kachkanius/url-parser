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
#include <QLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(this->size());

    setupTable();

    ui->edit_max_urls->setValidator(new QIntValidator(0, INT_MAX, this));
    ui->edit_threads_num->setValidator(new QIntValidator(0, 100, this));

    connect(ui->button_start, SIGNAL (released()), this, SLOT (startButtonPressed()));
    connect(ui->button_stop, SIGNAL (released()), this, SLOT (stopButtonPressed()));

//    connect(&m_manager, SIGNAL (addItem(QString, int)), this, SLOT (addItem(QString, int)));
    connect(&m_manager, SIGNAL (updateItem(int, PageLoader::Status)), this, SLOT (updateItem(int, PageLoader::Status)));
    connect(&m_manager, SIGNAL (stateChanged(Manager::State)), this, SLOT (updateUi(Manager::State)));



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTable()
{
    ;
    QStringList headerLabels;
    headerLabels << "Url" << "Status";

    ui->table->setColumnCount(headerLabels.size());
    ui->table->setHorizontalHeaderLabels(headerLabels);
    ui->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Fixed);
    ui->table->setColumnWidth(1, 100);
}

QString MainWindow::getStringStatus(PageLoader::Status st)
{
    switch (st) {
    case PageLoader::Status::FOUND:
        return "FOUND!";
        break;
    case PageLoader::Status::HTTP_ERROR:
        return "Http Error";
        break;
    case PageLoader::Status::NOT_FOUND:
        return "Not found";
        break;
    case PageLoader::Status::LOADING:
    default:
        return "Loading...";
    }
}

void MainWindow::addThread(int id)
{
    PageLoader* worker = new PageLoader(ui->edit_start_url->text(), ui->edit_text_to_find->text(), 0);
    QThread* thread = new QThread;
    worker->setId(id); /* передаем список файлов для обработки */
    worker->moveToThread(thread);

    /*  Теперь внимательно следите за руками.  Раз: */
        connect(thread, SIGNAL(started()), worker, SLOT(start()));
    /* … и при запуске потока будет вызван метод process(), который создаст построитель отчетов, который будет работать в новом потоке

    Два: */
        connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    /* … и при завершении работы построителя отчетов, обертка построителя передаст потоку сигнал finished() , вызвав срабатывание слота quit()

    Три:
    */
//        connect(this, SIGNAL(stopAll()), worker, SLOT(stop()));
    /* … и Session может отправить сигнал о срочном завершении работы обертке построителя, а она уже остановит построитель и направит сигнал finished() потоку

    Четыре: */
        connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    /* … и обертка пометит себя для удаления при окончании построения отчета

    Пять: */
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    /* … и поток пометит себя для удаления, по окончании построения отчета. Удаление будет произведено только после полной остановки потока.
*/


    connect(worker, SIGNAL (loaded (int,  QString, PageLoader::Status)),
            this, SLOT (addItem(int,  QString, PageLoader::Status)));

        qDebug () << "starting thread " << id;
        thread->start();
    /* Запускаем поток, он запускает RBWorker::process(), который создает ReportBuilder и запускает  построение отчета */

}



void MainWindow::startButtonPressed() {
//    m_manager.setMaxScannedLinks(ui->edit_max_urls->text().toInt());
//    m_manager.setMaxThreadsCount(ui->edit_threads_num->text().toInt());
//    m_manager.start(ui->edit_start_url->text(), ui->edit_text_to_find->text());
addThread(0);
addThread(1);
addThread(2);
addThread(3);
addThread(4);
addThread(5);
addThread(6);
addThread(7);
addThread(8);
addThread(9);


}

void MainWindow::stopButtonPressed()
{
    m_manager.stop();
}

void MainWindow::updateItem(int id, PageLoader::Status status)
{
//    QMutexLocker locker(&m_tableMutex);
    if (ui->table->rowCount() > id)
    {
        //The table takes ownership of the item.
        ui->table->setItem(id, 1,  new QTableWidgetItem( getStringStatus(status)));
        if (status != PageLoader::Status::LOADING) {
            int progress = ((id +1)* 100) / m_manager.getMaxScannedLinks();
            ui->progressBar->setValue(progress);
        }
    }
}

void MainWindow::addItem(int id, QString url, PageLoader::Status st)
{
    QMutexLocker locker(&m_tableMutex);
    // TODO: fix warning
    //if (ui->table->rowCount() <= id)
    {
        int row = ui->table->rowCount();
        ui->table->insertRow(ui->table->rowCount());
        //The table takes ownership of the item.
        ui->table->setItem(row, 0, new QTableWidgetItem( url + " "+ QString::number(id)));
        ui->table->setItem(row, 1, new QTableWidgetItem( getStringStatus(st)));
    }
}

void MainWindow::updateUi(Manager::State newState)
{
    switch (newState) {
    case Manager::State::STOPPED:        
        ui->table->setRowCount(0);
        ui->progressBar->setValue(0);
    case Manager::State::FINISHED:
        ui->button_start->setText("Start");
        ui->edit_max_urls->setEnabled(true);
        ui->edit_start_url->setEnabled(true);
        ui->edit_text_to_find->setEnabled(true);
        ui->edit_threads_num->setEnabled(true);
        break;
    case Manager::State::RUNNING:
        ui->button_start->setText("Pause");
        ui->edit_max_urls->setEnabled(false);
        ui->edit_start_url->setEnabled(false);
        ui->edit_text_to_find->setEnabled(false);
        ui->edit_threads_num->setEnabled(false);
        break;
    case Manager::State::PAUSED:
        ui->button_start->setText("Resume");
        ui->edit_max_urls->setEnabled(false);
        ui->edit_start_url->setEnabled(false);
        ui->edit_text_to_find->setEnabled(false);
        ui->edit_threads_num->setEnabled(false);
        break;
    default:
        break;
    }
}


