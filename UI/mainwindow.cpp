#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMovie>

#include <PageParser.h>
#include <PageLoader.h>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_finishedItemsCount(0)
{
    ui->setupUi(this);

    this->setFixedSize(this->size());

    setupTable();

    ui->edit_max_urls->setValidator(new QIntValidator(0, INT_MAX, this));
    ui->edit_threads_num->setValidator(new QIntValidator(0, 100, this));

    m_loadingMovie = new QMovie(":/images/loader.gif");
    ui->loading_bar->setMovie(m_loadingMovie);
    ui->loading_bar->show();
//    m_loadingMovie->start();

    connect(ui->button_start, SIGNAL (released()), this, SLOT (startButtonPressed()));
    connect(ui->button_stop, SIGNAL (released()), this, SLOT (stopButtonPressed()));

    connect(&m_manager, SIGNAL (addItem(QString)), this, SLOT (onAddItem(QString)));
    connect(&m_manager, SIGNAL (updateItem(int, PageLoader::Status, QString)), this, SLOT (onUpdateItem(int, PageLoader::Status, QString)));
    connect(&m_manager, SIGNAL (stateChanged(Manager::State)), this, SLOT (onStateChanged(Manager::State)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_loadingMovie;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Key pressed!";
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        startButtonPressed();
    }
}

void MainWindow::setupTable()
{
    QStringList headerLabels;
    headerLabels << "Url" << "Status";

    ui->table->setColumnCount(headerLabels.size());
    ui->table->setHorizontalHeaderLabels(headerLabels);
    ui->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
}

QString MainWindow::getStringStatus(PageLoader::Status st, QString err)
{
    switch (st) {
    case PageLoader::Status::FOUND:
        return "Found!";
        break;
    case PageLoader::Status::HTTP_ERROR:
        return err;
        break;
    case PageLoader::Status::NOT_FOUND:
        return "Not found";
        break;
    case PageLoader::Status::HTTP_NO_TEXT:
        return "Not a text";
        break;
    case PageLoader::Status::LOADING:
    default:
        return "Loading...";
    }
}

void MainWindow::updateUi(bool enabled)
{
    ui->edit_max_urls->setEnabled(enabled);
    ui->edit_start_url->setEnabled(enabled);
    ui->edit_text_to_find->setEnabled(enabled);
    ui->edit_threads_num->setEnabled(enabled);
    ui->caseSensitive->setEnabled(enabled);
}


void MainWindow::startButtonPressed()
{
    m_manager.setMaxScannedLinks(ui->edit_max_urls->text().toInt());
    m_manager.setMaxThreadsCount(ui->edit_threads_num->text().toInt());
    m_manager.start(ui->edit_start_url->text(), ui->caseSensitive->isChecked(), ui->edit_text_to_find->text());
}

void MainWindow::stopButtonPressed()
{
    m_manager.stop();
    m_finishedItemsCount = 0;
}

void MainWindow::onUpdateItem(int id, PageLoader::Status status, QString err)
{
    QMutexLocker locker(&m_tableMutex);
    if (ui->table->rowCount() > id)
    {
        ++m_finishedItemsCount;
        //The table takes ownership of the item.
        ui->table->setItem(id, 1,  new QTableWidgetItem(getStringStatus(status, err)));
        if (status != PageLoader::Status::LOADING) {
            int progress = ((m_finishedItemsCount)* 100) / m_manager.getMaxScannedLinks();
            ui->progressBar->setValue(progress);
        }
    }
}

void MainWindow::onAddItem(QString url)
{
    QMutexLocker locker(&m_tableMutex);
    int row = ui->table->rowCount();
    ui->table->insertRow(row);
    //The table takes ownership of the item.
    ui->table->setItem(row, 0, new QTableWidgetItem(url));
    ui->table->setItem(row, 1, new QTableWidgetItem(getStringStatus(PageLoader::LOADING)));
}

void MainWindow::onStateChanged(Manager::State newState)
{

    switch (newState) {
    case Manager::State::STOPPED:
        updateUi(true);
        ui->button_start->setText("Start");
        ui->button_start->setEnabled(true);
        ui->table->setRowCount(0);
        ui->progressBar->setValue(0);
        ui->loading_bar->setVisible(false);
        m_finishedItemsCount = 0;
        break;
   case Manager::State::FINISHED:
        updateUi(true);
        ui->button_start->setText("Start");
        ui->button_start->setEnabled(true);
        m_loadingMovie->setPaused(true);
        break;
    case Manager::State::WAITING:
        updateUi(false);
        ui->button_start->setText("Pause");
        ui->button_start->setEnabled(false);
        m_loadingMovie->setPaused(false);
        break;
    case Manager::State::RUNNING:
        updateUi(false);
        ui->button_start->setText("Pause");
        ui->button_start->setEnabled(true);
        ui->loading_bar->setVisible(true);
        m_loadingMovie->setPaused(false);
        break;
    case Manager::State::PAUSED:
        updateUi(false);
        ui->button_start->setText("Resume");
        ui->button_start->setEnabled(true);
        m_loadingMovie->setPaused(true);
        break;
    default:
        break;
    }
}


