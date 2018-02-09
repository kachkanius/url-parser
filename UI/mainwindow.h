#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    int m_finishedItemsCount;
private:
    void setupTable();
    QString getStringStatus(PageLoader::Status st);
    void lockUi(bool locked);

private slots:
    void startButtonPressed();
    void stopButtonPressed();
    void onAddItem(QString url);
    void onUpdateItem(int id,  PageLoader::Status status);
    void onStateChanged(Manager::State newState);
};

#endif // MAINWINDOW_H
