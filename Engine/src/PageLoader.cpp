#include "PageLoader.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRunnable>
#include <QThread>
#include <QEventLoop>

PageLoader::PageLoader(const QString &sUrl, const QString &text, int depth)
    : m_request(QUrl(sUrl))
    , m_textToFind(text)
    , m_depth(depth)
    , m_id(0)
{
    m_request.setRawHeader("User-Agent", "Mozilla/5.0 (Android; Mobile; rv:40.0) Gecko/40.0 Firefox/40.0");
}

QString PageLoader::getUrl() const
{
    return m_request.url().toString();
}

size_t PageLoader::getId() const
{
    return m_id;
}

void PageLoader::setId(size_t id)
{
    m_id = id;
}

PageLoader::~PageLoader() {
    qDebug() << "~PageLoader()" <<m_id;
}

void PageLoader::start()
{
    PageLoader::Status operationStatus = HTTP_ERROR;
    QStringList urls;

    QNetworkAccessManager m_netwManager;

    qDebug() << "thread started, GEt request# "<< m_id;
    if (m_id == 0) {
        QThread::msleep(2000);
    } else {
        QThread::msleep(1000);
    }
    m_netwManager.get(m_request);


    QEventLoop loop;
    QNetworkReply *reply = m_netwManager.get(m_request);
//    QObject::connect(&m_netwManager, SIGNAL(finished(QNetworkReply *)),
//                     this, SLOT(httpFinish(QNetworkReply *)));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();


    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "reply status " << httpStatus;
    if (httpStatus == 200) {
        operationStatus = NOT_FOUND;
        m_body = reply->readAll();
        QString body_str(m_body);
        PageParser parser(body_str);
        urls = parser.getUrls();
        if (parser.containsStr(m_textToFind)) {
            operationStatus = FOUND;
        }
    }
//    qDebug() << "getPage() HTTP status: " << httpStatus << m_url;
//    if (httpStatus == 301) {
//        m_url = QString (netwReply->rawHeader("Location"));
//        return getPage();
//    }

    reply->deleteLater();
//    emit pageLoaded(operationStatus, urls, m_id, m_depth);
    emit loaded(m_id, m_request.url().toString(), operationStatus);
    emit finished();

//    qDebug() << "getPage() HTTP status: " << httpStatus << m_url;
//    if (httpStatus == 301) {
//        m_url = QString (netwReply->rawHeader("Location"));
//        return getPage();
//    }
}


void PageLoader::httpFinish(QNetworkReply *reply)
{
    qDebug() << "httpFinish()";

    PageLoader::Status operationStatus = HTTP_ERROR;
    QStringList urls;

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatus == 200) {
        operationStatus = NOT_FOUND;
        m_body = reply->readAll();
        QString body_str(m_body);
        PageParser parser(body_str);
        urls = parser.getUrls();
        if (parser.containsStr(m_textToFind)) {
            operationStatus = FOUND;
        }
    }
//    qDebug() << "getPage() HTTP status: " << httpStatus << m_url;
//    if (httpStatus == 301) {
//        m_url = QString (netwReply->rawHeader("Location"));
//        return getPage();
//    }

    reply->deleteLater();
    emit pageLoaded(operationStatus, urls, m_id, m_depth);
    emit finished();
}

//int PageLoader::getPage()
//{
//    QEventLoop loop;
//    QNetworkAccessManager nam;
//    QNetworkRequest req(m_url);
//    req.setRawHeader("User-Agent", "Mozilla/5.0 (Android; Mobile; rv:40.0) Gecko/40.0 Firefox/40.0");
//    QNetworkReply *netwReply = nam.get(req);
//    QObject::connect(netwReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
//    loop.exec();
//    int httpStatus = netwReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//    m_body = netwReply->readAll();
//    qDebug() << "getPage() HTTP status: " << httpStatus << m_url;
//    if (httpStatus == 301) {
//        m_url = QString (netwReply->rawHeader("Location"));
//        return getPage();
//    }
//    return httpStatus;
//}

