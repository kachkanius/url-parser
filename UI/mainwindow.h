#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <PageLoader.h>
//#include <QMutex>

#include <Manager.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Manager m_manager;
    QMutex m_tableMutex;
private:
    void setupTable();
    QString getStringStatus(PageLoader::Status st);
    void addThread(int id);

private slots:
    void startButtonPressed();
    void stopButtonPressed();
    void updateItem(int id,  PageLoader::Status status);
    void addItem(int id, QString url, PageLoader::Status st);
    void updateUi(Manager::State newState);
};

#endif // MAINWINDOW_H
