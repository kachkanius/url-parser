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
//    ui->text_browser->setEnabled(false);

    ui->edit_max_urls->setValidator(new QIntValidator(0, INT_MAX, this));
    ui->edit_threads_num->setValidator(new QIntValidator(0, 100, this));

    connect(ui->button_start, SIGNAL (released()), this, SLOT (startButtonPressed()));
    connect(ui->button_stop, SIGNAL (released()), this, SLOT (stopButtonPressed()));

    connect(&m_manager, SIGNAL (addItem(QString, size_t)), this, SLOT (addItem(QString, size_t)));
    connect(&m_manager, SIGNAL (updateItem(size_t, PageLoader::Status)), this, SLOT (updateItem(size_t, PageLoader::Status)));
    connect(&m_manager, SIGNAL (stateChanged(Manager::State)), this, SLOT (updateUi(Manager::State)));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startButtonPressed() {
    m_manager.setMaxScannedLinks(ui->edit_max_urls->text().toInt());
    m_manager.setMaxThreadsCount(ui->edit_threads_num->text().toInt());
    m_manager.start(ui->edit_start_url->text(), ui->edit_text_to_find->text());
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
    qDebug() <<  "Add item# " + QString::number(id);

    QString item(url + " " +  QString::number(id) + " LOADING ");
    ui->text_browser->append(item);
}

void MainWindow::updateUi(Manager::State newState)
{
    switch (newState) {
    case Manager::State::STOPPED:
        ui->text_browser->clear();
    case Manager::State::FINISHED:
        ui->button_start->setText("Start");
        ui->edit_max_urls->setEnabled(true);
        ui->edit_start_url->setEnabled(true);
        ui->edit_text_to_find->setEnabled(true);
        ui->edit_threads_num->setEnabled(true);
        break;
    case Manager::State::RUNNING:
    case Manager::State::PAUSED:
        ui->button_start->setText("Pause");
        ui->edit_max_urls->setEnabled(false);
        ui->edit_start_url->setEnabled(false);
        ui->edit_text_to_find->setEnabled(false);
        ui->edit_threads_num->setEnabled(false);
        break;
    default:
        break;
    }
}
