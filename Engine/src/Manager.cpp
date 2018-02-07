#include "Manager.h"
#include <QDebug>
#include <QThreadPool>
#include <mutex>

Manager::Manager(QObject *parent) : QObject(parent)
  , m_maxLinks(0)
  , m_linkNum(0)
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
    return m_maxLinks;
}

void Manager::setMaxScannedLinks(int links)
{
    m_maxLinks = links;
}

void Manager::setMaxThreadsCount(int threads)
{
    m_pool.setMaxThreadCount(10);
}

void Manager::start(const QString &startPage, const QString &strToFind)
{
    qDebug() <<"start button pressed!\n";
    QMutexLocker locker(&m_queueMutex);

    switch (m_state) {
    case State::RUNNING:
        m_state = State::PAUSED;
        break;
    case State::PAUSED:
        m_state = State::RUNNING;
        startHeadJob();
        break;
    default: //start again
    {
        m_state = State::RUNNING;
        cleanUp();
        m_strToFind = strToFind;
        PageLoader* firstItem = new PageLoader(startPage, m_strToFind, 0);
        QQueue<PageLoader*> zero;
        zero.enqueue(firstItem);
        m_grapth.push_back(zero);
        m_currentJobs = &m_grapth[0];
        startHeadJob();
    }
        break;
    }
    emit stateChanged(m_state);
}

void Manager::stop()
{
    QMutexLocker locker(&m_queueMutex);
    cleanUp();
    m_state = State::STOPPED;
    emit stateChanged(m_state);
}

void Manager::threadFinished(PageLoader::Status status, QStringList urls, int id, int depth)
{
    // Update status just downloaded page!
    emit updateItem(id, status);

//    QMutexLocker locker(&m_queueMutex);
    if (m_state != State::STOPPED) {
        if (status != PageLoader::Status::HTTP_ERROR) {
            if (m_grapth.size() <= depth + 1) {
                m_grapth.push_back(QQueue<PageLoader*>());
            }
            qDebug() << "adding " << urls.size() << "Urls";
            for (auto& it : urls) {
//                qDebug() << it;
                m_grapth[depth + 1].enqueue(new PageLoader(it, m_strToFind, depth + 1));
            }
        }
        if (m_currentJobs->empty() && m_grapth.size() > depth) {
            m_currentJobs = &m_grapth[depth + 1];
        }

        startHeadJob();
    }

}

void Manager::startHeadJob()
{
    if (m_currentJobs->isEmpty() || m_state != State::RUNNING) {
        return;
    }
    if (m_linkNum >= m_maxLinks) {
        qDebug() << "End searching!";
        m_pool.clear();
        m_state = State::FINISHED;
        emit stateChanged(m_state);
        return;
    }

    PageLoader* worker = m_currentJobs->dequeue();
    worker->setId(m_linkNum);

    QObject::connect(worker, SIGNAL(pageLoaded(PageLoader::Status, QStringList, int, int)),
                     this, SLOT(threadFinished(PageLoader::Status, QStringList, int, int)),
                     Qt::QueuedConnection);
    // thread pool takes ownership of worker here!
    if (!m_pool.tryStart(worker)) {
        m_currentJobs->enqueue(worker);
        qDebug() << "no free threads!!!!!!!!!!!!!!!!!!!!!!! ";
    } else {
        ++m_linkNum;
        emit addItem(worker->getUrl(), worker->getId());
    }

}

void Manager::cleanUp()
{
    m_pool.clear();
    for(auto& works : m_grapth) {
        for(auto& work : works) {
            delete work;
        }
    }
    m_grapth.clear();
    m_linkNum = 0;
}

