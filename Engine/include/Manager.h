#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <PageLoader.h>
#include <QMutex>
#include <QString>
#include <QVector>
#include <QQueue>

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);

    ~Manager();

    void setMaxScannedLinks(size_t links);
    void setMaxThreadsCount(int threads);

    void start(const QString& startPage, const QString& strToFind);
    void stop();
    void pause();
    void resume();
signals:
    void updateItem(size_t id,  PageLoader::Status status);
    void addItem(QString url, size_t id);

private slots:

    void threadFinished(PageLoader::Status status, QStringList urls, size_t id, int depth);


private:
    void startHeadJob();
    void cleanUp();
private:
    QQueue<PageLoader*>* m_currentJobs;
    QVector<QQueue<PageLoader*>> m_grapth;
    QMutex m_queueMutex;
    size_t m_maxLinks;
    size_t m_scannedLinks;
    size_t m_currentLink;
    QString m_strToFind;
};

#endif // MANAGER_H

