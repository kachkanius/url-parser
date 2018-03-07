#include "Manager.h"
#include <QDebug>
#include <mutex>

Manager::Manager(QObject *parent) : QObject(parent)
  , m_state(State::STOPPED)
  , m_caseSensitive(false)
  , m_maxLinksCount(0)
  , m_linkCount(0)
  , m_maxThreadsCount(1)
  , m_activeThreads(0)

{
    // register new type to handle qt signals/slots
    static std::once_flag onceFlag;
    std::call_once( onceFlag, [ ]
    {
        qRegisterMetaType<Manager::State>("Manager::State");
    } );
}

Manager::~Manager()
{
    cleanUp();
}

int Manager::getMaxScannedLinks() const
{
    return m_maxLinksCount;
}

void Manager::setMaxScannedLinks(int links)
{
    m_maxLinksCount = links;
}

void Manager::setMaxThreadsCount(int threads)
{
    m_maxThreadsCount = threads;
}

void Manager::start(const QString &startPage, bool caseSensitive, const QString &strToFind)
{
    qDebug() <<"start button pressed!\n";
    QMutexLocker locker(&m_queueMutex);

    switch (m_state) {
    case State::RUNNING: // Pause
        setState(State::PAUSED);
        break;
    case State::WAITING:
        break;
    case State::PAUSED: // Resume
        setState(State::RUNNING);
        startHeadJob();
        break;
    default: // start
    {
        // Remove/stop prev jobs.
        setState(State::STOPPED);
        setState(State::RUNNING);
        cleanUp();
        m_strToFind = strToFind;
        m_caseSensitive = caseSensitive;

        // Prepare first (root) vertex.
        Job firstItem(startPage, 0);
        QQueue<Job> zeroLevel;
        zeroLevel.enqueue(firstItem);
        m_grapth.push_back(zeroLevel);
        m_currentJobs = &m_grapth[0];
        // start BFS
        startHeadJob();
        break;
    }

    }
}

void Manager::stop()
{
    QMutexLocker locker(&m_queueMutex);
    setState(State::STOPPED);
    cleanUp();
}


void Manager::threadFinished(int id, PageLoader::Status status, QString err, QStringList urls, int depth)
{
    QMutexLocker locker(&m_queueMutex);
    qDebug() << "Manager::threadFinished (" << id << ") status: " << status;
    // Update UI
    emit updateItem(id, status, err);

    --m_activeThreads;

    if (m_state != State::STOPPED) {
        // Add just parsed urls to grapth
        if (m_grapth.size() <= depth + 1) {
            m_grapth.push_back(QQueue<Job>());
        }

        // Add founded url to queue of derived branch
        for (auto& it : urls)
        {
            m_grapth[depth + 1].enqueue(Job(it,depth + 1));
        }
        qDebug() << urls.size() << "urls were added.";

        // Check if all jobs on this level were started/finished.
        // If so, set next level as current.
        if (m_currentJobs->empty()) {
            if (m_grapth.size() > depth)
            {
                qDebug() << "Go to next  Level "<< depth + 1;
                m_currentJobs = &m_grapth[depth + 1];
            } else {
                qDebug() << "No Urls in grapth.";
                setState(State::WAITING);
            }
        }

        if (m_state == State::RUNNING) {
            int freeThreads = (m_maxThreadsCount - m_activeThreads);
            for (int i = 0; i <= freeThreads; ++i)
            {
                if (!startHeadJob())
                {
                    break;
                }
            }
        } else if ((m_state == State::WAITING) && (m_activeThreads == 0))
        {
            qDebug() << "All threads finished, finish.";
            setState(State::FINISHED);
            return;
        }
    }
}

bool Manager::startHeadJob()
{
    // Reached maximum number of scanned links.
    if (m_linkCount >= m_maxLinksCount)
    {
        qDebug() << "End searching!";
        setState(State::WAITING);
        return false;
    }

    // Not reached maximum number of scanned links but nothing to do.
    if (m_state == State::RUNNING && m_currentJobs->empty())
    {
        qDebug() << "No jobs any more at this level!";
        return false;
    }

    Job job = m_currentJobs->dequeue();
    // PageLoader works works asynchronously, so we do't create thread-pool here.
    // PageLoader will be deleted automatcly at the end of operation by calling deleteLater();
    PageLoader* worker = new PageLoader(job.url, m_strToFind, m_caseSensitive, job.depth);
    worker->setId(m_linkCount);
    ++m_linkCount;
    ++m_activeThreads;

    // Update UI
    emit addItem(job.url);

    // Notify Manager that loading done
    connect(worker, SIGNAL(pageLoaded(int, PageLoader::Status, QString, QStringList, int)),
            this, SLOT(threadFinished(int, PageLoader::Status, QString, QStringList, int)));

    // Force stop downloading.
    connect(this, SIGNAL(stopAllThreads()), worker, SLOT(stop()));

    worker->start();
    return true;
}

void Manager::cleanUp()
{
    m_activeThreads = 0;
    m_grapth.clear();
    m_linkCount = 0;
    emit stopAllThreads();
}

void Manager::setState(Manager::State state)
{
    qDebug() << "state: "<< (int)state;
    m_state = state;
    emit stateChanged(m_state);
}
