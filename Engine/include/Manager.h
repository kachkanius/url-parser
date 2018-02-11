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
    enum class State {
        STOPPED = 0,
        RUNNING = 1,
        PAUSED = 2,
        FINISHED = 3,
        WAITING = 4
    };

    explicit Manager(QObject *parent = nullptr);

    ~Manager();

    void setMaxScannedLinks(int links);
    int getMaxScannedLinks() const;
    int getScannedLinks();
    void setMaxThreadsCount(int threads);

    void start(const QString& startPage, bool caseSensitive, const QString& strToFind);
    void stop();

signals:
    void updateItem(int id,  PageLoader::Status status, QString err);
    void addItem(QString url);
    void stateChanged(Manager::State state);
    void stopAllThreads();

private slots:
    void threadFinished(int id, PageLoader::Status status, QString err, QStringList urls, int depth);
private:
    bool startHeadJob();
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
    bool m_caseSensitive;
    State m_state;
};

#endif // MANAGER_H

