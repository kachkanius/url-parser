#include "Manager.h"
#include <QDebug>
#include <QThreadPool>
#include <mutex>

Manager::Manager(QObject *parent) : QObject(parent)
  , m_maxLinks(0)
  , m_scannedLinks(0)
  , m_currentLink(0)
{
    static std::once_flag onceFlag;
    std::call_once( onceFlag, [ ]
    {
        qRegisterMetaType<PageLoader::Status>("PageLoader::Status");
        qRegisterMetaType<size_t>("size_t");
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
    // clear state
    cleanUp();

    m_strToFind = strToFind;

    PageLoader* firstItem = new PageLoader(startPage, m_strToFind, 0);
    QQueue<PageLoader*> zero;
    zero.enqueue(firstItem);
    m_grapth.push_back(zero);
    m_currentJobs = &m_grapth[0];

    startHeadJob();
}

void Manager::stop()
{
    QMutexLocker locker(&m_queueMutex);
    cleanUp();
}

void Manager::pause()
{
    //TODO
}

void Manager::resume()
{
    //TODO
}

void Manager::threadFinished(PageLoader::Status status, QStringList urls, size_t id, int depth)
{
    QMutexLocker locker(&m_queueMutex);
    qDebug() <<"GOT URLS in main thread:";

    // Update status just downloaded page!
    emit updateItem(id, status);

    if (status != PageLoader::Status::HTTP_ERROR) {
        if (m_grapth.size() <= depth + 1) {
            m_grapth.push_back(QQueue<PageLoader*>());
        }

        for (auto& it : urls) {
            qDebug() << it;
//            ++m_scannedLinks;
            m_grapth[depth + 1].enqueue(new PageLoader(it, m_strToFind, depth + 1));

//            if (m_scannedLinks > m_maxLinks) {
//                qDebug() << "Stop LOOP!!";
//                QThreadPool::globalInstance()->clear();
//                return;
//            }
        }
    }
    if (m_currentJobs->empty() && m_grapth.size() > depth) {
        m_currentJobs = &m_grapth[depth + 1];
    }

    startHeadJob();
}

void Manager::startHeadJob()
{
    if (m_currentJobs->isEmpty()) {
        return;
    }
    ++m_currentLink;
    if (m_currentLink > m_maxLinks) {
        qDebug() << "Stop LOOP!!";
        QThreadPool::globalInstance()->clear();
        return;
    }

    PageLoader* worker = m_currentJobs->dequeue();
    worker->setId(m_currentLink);

    QObject::connect(worker, SIGNAL(pageLoaded(PageLoader::Status, QStringList, size_t, int)),
                     this, SLOT(threadFinished(PageLoader::Status, QStringList, size_t, int)),
                     Qt::QueuedConnection);
    // Notify that start download page!
    emit addItem(worker->getUrl(), worker->getId());

    QThreadPool::globalInstance()->start(worker);
}

void Manager::cleanUp()
{
    QThreadPool::globalInstance()->clear();
    for(auto& works : m_grapth) {
        for(auto& work : works) {
            delete work;
        }
    }
    m_grapth.clear();
    m_scannedLinks = 0;
    m_currentLink = 0;
}

