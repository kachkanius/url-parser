#include "PageLoader.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRunnable>
#include <QThread>
#include <QEventLoop>

PageLoader::PageLoader(const QString &sUrl, const QString &text, int depth)
    : m_url(sUrl)
    , m_depth(depth)
    , m_str(text)
    , m_id(0)
{
    this->setAutoDelete(true);
}

QString PageLoader::getUrl() const
{
    return m_url.toString();
}

size_t PageLoader::getId() const
{
    return m_id;
}

void PageLoader::setId(size_t id)
{
    m_id = id;
}

void PageLoader::run()
{
    QString thread_str = QString::number(*(u_int64_t*)QThread::currentThreadId());

    qDebug() << "New thread: " << thread_str <<", URL: " << m_url;

    int httpStatus = getPage();

    Status status = HTTP_ERROR;
    QStringList urls;

    if (httpStatus == 200) {        
        status = NOT_FOUND;
        QString body_str(m_body);
        PageParser parser(body_str);
        urls = parser.getUrls();
        if (parser.containsStr(m_str)) {
            status = FOUND;
        }
    }
    emit pageLoaded(status, urls, m_id, m_depth);
    qDebug() << "Thread " + thread_str << " will be destroyed now...";
}

int PageLoader::getPage()
{
    QEventLoop loop;
    QNetworkAccessManager nam;
    QNetworkRequest req(m_url);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Android; Mobile; rv:40.0) Gecko/40.0 Firefox/40.0");
    QNetworkReply *netwReply = nam.get(req);
    QObject::connect(netwReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    int httpStatus = netwReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_body = netwReply->readAll();
    qDebug() << "getPage() HTTP status: " << httpStatus << m_url;
    if (httpStatus == 301) {
        m_url = QString (netwReply->rawHeader("Location"));
        return getPage();
    }
    return httpStatus;
}
PageLoader::~PageLoader() {
    qDebug() << "~PageLoader()";
}

