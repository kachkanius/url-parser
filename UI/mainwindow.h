#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <PageLoader.h>

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
    QTimer *mainLoopTimer;
    QQueue<PageLoader*>* jobs;
    QVector<QQueue<PageLoader*>> grapth;
    int count;

private slots:
    void handleButton();
    void threadFinished(QStringList urls, int depth);
};

#endif // MAINWINDOW_H
