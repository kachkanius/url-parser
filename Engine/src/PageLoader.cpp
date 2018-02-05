#include "PageLoader.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRunnable>
#include <QThread>
#include <QEventLoop>


#include <QFile>

PageLoader::PageLoader(const QString &sUrl, int depth) :m_url(sUrl), m_depth(depth)
{
    this->setAutoDelete(true);
}

void PageLoader::run()
{
    QString thread_str = QString::number(*(u_int64_t*)QThread::currentThreadId());

    qDebug() << "New thread: " << thread_str <<", URL: " << m_url;

    int httpStatus = getPage();

    QFile file(thread_str);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(m_body);
    file.flush();
    file.close();

    QString body(m_body);

    if (httpStatus == 200) {
        PageParser parser(body);
        QStringList urls = parser.getUrls();
        qDebug() <<"thread id: " << thread_str << "parsed urls:";

        for (auto& it : urls) {
            qDebug() << it;
        }

        emit finished(urls, m_depth);
    }
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

