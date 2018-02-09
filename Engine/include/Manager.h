#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <PageLoader.h>
#include <QMutex>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QThreadPool>
#include <QTimer>

class Manager : public QObject
{
    Q_OBJECT
public:
    enum class State {
        STOPPED,
        RUNNING,
        PAUSED,
        FINISHED
    };

    explicit Manager(QObject *parent = nullptr);

    ~Manager();

    void setMaxScannedLinks(int links);
    int getMaxScannedLinks() const;
    int getScannedLinks();
    void setMaxThreadsCount(int threads);

    void start(const QString& startPage, const QString& strToFind);
    void stop();

signals:
    void updateItem(int id,  PageLoader::Status status);
    void addItem(QString url);
    void stateChanged(Manager::State state);

private slots:
    void threadFinished(int id, PageLoader::Status status, QStringList urls, int depth);
private:
    void startHeadJob();
    void cleanUp();
    void setState(State state);
private:
    struct Job {
        QString url;
        int depth;
        Job(const QString& url_, int depth_):url(url_), depth(depth_) {}
    };
    QQueue<Job>* m_currentJobs;
    QVector<QQueue<Job>> m_grapth;
    QMutex m_queueMutex;
    int m_maxLinksCount;
    int m_linkCount;
    int m_maxThreadsCount;
    int m_activeThreads;
    QString m_strToFind;
    State m_state;
    QTimer m_timer;
};

#endif // MANAGER_H

