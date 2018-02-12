#include "PageLoader.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <mutex>

PageLoader::PageLoader(const QString &sUrl, const QString &text, bool caseSensitive, int depth)
    : m_request(QUrl(sUrl))
    , m_textToFind(text)
    , m_depth(depth)
    , m_id(0)
    , m_isActive(true)
    , m_caseSensitive(caseSensitive)
{
    m_request.setRawHeader("User-Agent", "Mozilla/5.0 (Android; Mobile; rv:40.0) Gecko/40.0 Firefox/40.0");
    // download only small pages up to 10 MB
    m_request.setRawHeader("Range", "bytes=0-10485760");

    connect(&m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestEnd(QNetworkReply*)));
    static std::once_flag onceFlag;
    std::call_once( onceFlag, [ ]
    {
        qRegisterMetaType<PageLoader::Status>("PageLoader::Status");
    } );
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
    connect(this, SIGNAL(cancelDownloading()), reply, SLOT(abort()));
}

void PageLoader::stop()
{
    emit cancelDownloading();
}

void PageLoader::requestEnd(QNetworkReply *reply)
{
    if (m_isActive)
    {
        PageLoader::Status operationStatus = Status::HTTP_ERROR;
        QStringList urls;

        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString contentType = QString(reply->rawHeader("Content-Type"));
        qDebug() << "httpFinish(" << reply->url().toString() << ") status " << httpStatus
                 << " Type: "<<  contentType;

        if (httpStatus == 200 || httpStatus == 206) // OK
        {
            if (contentType.contains("text"))
            {
                operationStatus = Status::NOT_FOUND;
                QString body = QString(reply->readAll());
                PageParser parser(body);
                urls = parser.getUrls();
                if (parser.containsStr(m_textToFind, m_caseSensitive)) {
                    operationStatus = Status::FOUND;
                }
            }
            else {
                qDebug() << "Don't have text!";
                operationStatus = Status::HTTP_NO_TEXT;
            }
        } else if (httpStatus == 301 || httpStatus == 302)  // Moved
        {
            m_request.setUrl(QString (reply->rawHeader("Location")));
            reply->deleteLater();
            start();
            return;
        }
        // Remove url name from error description
        QString errr = reply->errorString();
        QString url = reply->url().toString();
        int pos = url.indexOf("://", 0);
        url.remove(0, pos + 3);
        errr.replace(url, "");

        emit pageLoaded(m_id, operationStatus, errr, urls, m_depth);
    } else {
        qDebug() << "httpFinish(" << reply->url().toString() << ") Operation was canceled";
    }
    reply->deleteLater();
    this->deleteLater();
}

void PageLoader::networkError(QNetworkReply::NetworkError err)
{
    if (err == QNetworkReply::NetworkError::OperationCanceledError) {
        m_isActive = false;
    }

    qDebug() << "Network Error(" << m_request.url().toString() << ") status " << err;
}
