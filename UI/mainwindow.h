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
    QMutex m_textMutex;
private:
//    void doHeadJob();

private slots:
    void startButtonPressed();
    void stopButtonPressed();
    void updateItem(size_t id,  PageLoader::Status status);
    void addItem(QString url, size_t id);
};

#endif // MAINWINDOW_H
