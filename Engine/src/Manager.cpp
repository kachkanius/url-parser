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
        qRegisterMetaType<size_t>("size_t");
        qRegisterMetaType<Manager::State>("Manager::State");
    } );
}

Manager::~Manager()
{
    cleanUp();
}

void Manager::setMaxScannedLinks(size_t links)
{
    m_maxLinks = links;
}

void Manager::setMaxThreadsCount(int threads)
{
    QThreadPool::globalInstance()->setMaxThreadCount(threads);
}

void Manager::start(const QString &startPage, const QString &strToFind)
{
    qDebug() <<"start button pressed!\n";
    QMutexLocker locker(&m_queueMutex);

    if (m_state != State::PAUSED) { //start again
        // clear state
        cleanUp();

        m_strToFind = strToFind;

        PageLoader* firstItem = new PageLoader(startPage, m_strToFind, 0);
        QQueue<PageLoader*> zero;
        zero.enqueue(firstItem);
        m_grapth.push_back(zero);
        m_currentJobs = &m_grapth[0];
        startHeadJob();
        m_state = State::RUNNING;
    } else { // resume
        m_state = State::RUNNING;
        startHeadJob();
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

void Manager::threadFinished(PageLoader::Status status, QStringList urls, size_t id, int depth)
{
    QMutexLocker locker(&m_queueMutex);
    qDebug() <<"GOT URLS in main thread:";
    // Update status just downloaded page!
    emit updateItem(id, status);

    if (m_state == State::RUNNING) {
        if (status != PageLoader::Status::HTTP_ERROR) {
            if (m_grapth.size() <= depth + 1) {
                m_grapth.push_back(QQueue<PageLoader*>());
            }

            for (auto& it : urls) {
                qDebug() << it;
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
    if (m_currentJobs->isEmpty()) {
        return;
    }
    ++m_linkNum;
    if (m_linkNum > m_maxLinks) {
        qDebug() << "End searching!";
        QThreadPool::globalInstance()->clear();
        m_state = State::FINISHED;
        emit stateChanged(m_state);
        return;
    }

    PageLoader* worker = m_currentJobs->dequeue();
    worker->setId(m_linkNum);

    QObject::connect(worker, SIGNAL(pageLoaded(PageLoader::Status, QStringList, size_t, int)),
                     this, SLOT(threadFinished(PageLoader::Status, QStringList, size_t, int)),
                     Qt::QueuedConnection);
    // Notify that start download page!
    emit addItem(worker->getUrl(), worker->getId());

    // thread puul takes ownership here!
    QThreadPool::globalInstance()->start(worker);
}

void Manager::cleanUp()
{
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone(-1);
    for(auto& works : m_grapth) {
        for(auto& work : works) {
            delete work;
        }
    }
    m_grapth.clear();
    m_linkNum = 0;
}

