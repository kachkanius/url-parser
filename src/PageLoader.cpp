#include "PageLoader.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRunnable>
#include <QThread>
#include <QEventLoop>

PageLoader::PageLoader(const QString &sUrl) :m_url(sUrl)
{
    this->setAutoDelete(true);
}

void PageLoader::run()
{
    QString thread_str = QString::number(*(u_int64_t*)QThread::currentThreadId());

    qDebug() << "Url: " << m_url << ", thread id: " << thread_str;
    QEventLoop loop;
    QNetworkAccessManager nam;
    QNetworkRequest req(m_url);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Android; Mobile; rv:40.0) Gecko/40.0 Firefox/40.0");
    QNetworkReply *netwReply = nam.get(req);
    QObject::connect(netwReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QString body(netwReply->readAll());
    // TODO(): add check status for 301 etc
    //        qDebug() << reply->rawHeader("Location");

    int httpStatus = netwReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();;
    qDebug() << "HTTP status: " << httpStatus;



    if (httpStatus == 200) {
        PageParser parser(body);
        QStringList urls = parser.getUrls();
        emit finished(urls);
    }
    qDebug() << "Thread " + thread_str << " will be destroyed now...";
}
PageLoader::~PageLoader() {
    qDebug() << "~PageLoader()";
}

