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
    connect(&m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestEnd(QNetworkReply*)));
}

QString PageLoader::getUrl() const
{
    return m_request.url().toString();
}

void PageLoader::setId(int newId)
{
    m_id = newId;
}

int PageLoader::getId() const
{
    return m_id;
}

PageLoader::~PageLoader() {
    qDebug() << "~PageLoader(" << m_request.url().toString() <<")";
}

void PageLoader::start()
{
    QNetworkReply* reply = m_netwManager.get(m_request);
    // error handling
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void PageLoader::requestEnd(QNetworkReply *reply)
{
    PageLoader::Status operationStatus = HTTP_ERROR;
    QStringList urls;

    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "httpFinish(" << reply->url().toString() << ") status " << httpStatus;
    if (httpStatus == 200) // OK
    {
        operationStatus = NOT_FOUND;
        QString body = QString(reply->readAll());
        PageParser parser(body);
        urls = parser.getUrls();
        if (parser.containsStr(m_textToFind)) {
            operationStatus = FOUND;
        }
    } else if (httpStatus == 301 || httpStatus == 302)  // Moved
    {
        m_request.setUrl(QString (reply->rawHeader("Location")));
        reply->deleteLater();
        start();
        return;
    }

    emit pageLoaded(m_id, operationStatus, urls, m_depth);
    reply->deleteLater();
    this->deleteLater();
}

void PageLoader::networkError(QNetworkReply::NetworkError err)
{
    qDebug() << "Network Error(" << m_request.url().toString() << ") status " << err;
}
