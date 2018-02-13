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
    // states of manager( Simplest realisation of state-machine)
    enum class State : int {
        STOPPED = 0, // Initial state. Do nothing.
        RUNNING = 1, //  Iterate over graph and go link by link.
        PAUSED = 2, // Paused by user, all currently running tasks will be finished
        FINISHED = 3, // All done (max urls count were reached or no more links to be scanned).
        WAITING = 4 // Max urls were reached/ no more links, but wait till last one will be finished.
    };
    Q_ENUM(State)
    explicit Manager(QObject *parent = nullptr);

    ~Manager();

    // getters/setters
    void setMaxScannedLinks(int links);
    int getMaxScannedLinks() const;
    void setMaxThreadsCount(int threads);

    // Start searching
    void start(const QString& startPage, bool caseSensitive, const QString& strToFind);
    // Force stop searching
    void stop();

signals:
    // Update UI
    void updateItem(int id,  PageLoader::Status status, QString err);
    void addItem(QString url);
    void stateChanged(Manager::State state);
    // Force stop downloading
    void stopAllThreads();

private slots:
    // Callback when downloading done
    void threadFinished(int id, PageLoader::Status status, QString err, QStringList urls, int depth);

private:
    bool startHeadJob();
    void cleanUp();
    // Change inner state of manager, notify UI about that
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
    State m_state;
    QString m_strToFind;
    bool m_caseSensitive;
    int m_maxLinksCount;
    int m_linkCount;
    int m_maxThreadsCount;
    int m_activeThreads;    
};

#endif // MANAGER_H

