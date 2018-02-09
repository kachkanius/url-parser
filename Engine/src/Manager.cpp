#include "Manager.h"
#include <QDebug>
#include <mutex>

Manager::Manager(QObject *parent) : QObject(parent)
  , m_maxLinksCount(0)
  , m_linkCount(0)
  , m_maxThreadsCount(1)
  , m_activeThreads(0)
  , m_caseSensitive(false)
  , m_state(State::STOPPED)
{
    static std::once_flag onceFlag;
    std::call_once( onceFlag, [ ]
    {
        qRegisterMetaType<PageLoader::Status>("PageLoader::Status");
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

int Manager::getScannedLinks()
{
    return m_linkCount;
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
        setState(State::STOPPED);
        setState(State::RUNNING);
        cleanUp();
        m_strToFind = strToFind;
        m_caseSensitive = caseSensitive;
        Job firstItem(startPage, 0);
        QQueue<Job> zeroLevel;
        zeroLevel.enqueue(firstItem);
        m_grapth.push_back(zeroLevel);
        m_currentJobs = &m_grapth[0];
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
    emit stopAllThreads();
}


void Manager::threadFinished(int id, PageLoader::Status status, QStringList urls, int depth)
{
    QMutexLocker locker(&m_queueMutex);
    qDebug() << "Manager::threadFinished (" << id << ") status: " << status;
    emit updateItem(id, status);
    --m_activeThreads;

    if (m_state != State::STOPPED) {
        if (m_grapth.size() <= depth + 1) {
            m_grapth.push_back(QQueue<Job>());
        }
        for (auto& it : urls) {
            m_grapth[depth + 1].enqueue(Job(it,depth + 1));
        }
        qDebug() << urls.size() << "urls were added.";

        if (m_currentJobs->empty() && m_grapth.size() > depth) {
            m_currentJobs = &m_grapth[depth + 1];
        }
        if (m_state == State::RUNNING) {
            for (int i = 0; i <= (m_maxThreadsCount - m_activeThreads); ++i)
            {
                if (!startHeadJob())
                {
                    break;
                }
            }

        } else if ((m_state == State::WAITING) && (m_activeThreads == 0))
        {
            qDebug() << "No jobs, finish.";
            setState(State::FINISHED);
            return;
        }
    }
}

bool Manager::startHeadJob()
{
    if (m_linkCount >= m_maxLinksCount) {
        qDebug() << "End searching!";
        setState(State::WAITING);
        return false;
    }

    if (m_state != State::RUNNING || m_currentJobs->empty())
    {
        return false;
    }

    Job job = m_currentJobs->dequeue();
    PageLoader* worker = new PageLoader(job.url, m_strToFind, m_caseSensitive, job.depth);
    worker->setId(m_linkCount);
    ++m_linkCount;
    ++m_activeThreads;

    // Update UI
    emit addItem(job.url);

    // Notify Manager that loading done
    connect(worker, SIGNAL(pageLoaded(int, PageLoader::Status, QStringList, int)),
            this, SLOT(threadFinished(int, PageLoader::Status, QStringList, int)));

    // Notify Manager that loading done
    connect(this, SIGNAL(stopAllThreads()), worker, SLOT(stop()));

    worker->start();
    return true;
}

void Manager::cleanUp()
{
    m_grapth.clear();
    m_linkCount = 0;
}

void Manager::setState(Manager::State state)
{
    m_state = state;
    emit stateChanged(m_state);
}
