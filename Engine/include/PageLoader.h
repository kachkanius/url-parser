#ifndef PAGELOADER_H
#define PAGELOADER_H

#include <PageParser.h>
#include <QRunnable>
#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QWaitCondition>

class PageLoader : public QObject
{
public:
    enum Status {
        LOADING = 0,
        FOUND = 1,
        NOT_FOUND = 2,
        HTTP_ERROR = 3
    };
    Q_OBJECT
public:
    PageLoader(const QString& sUrl, const QString& text, int depth);
    ~PageLoader();

    QString getUrl() const;
    size_t getId() const;
    void setId(size_t id);

public slots:
        void start();

private slots:
    void httpFinish(QNetworkReply* reply);

signals:
    void pageLoaded(PageLoader::Status status, QStringList urls, int id, int depth);
    void loaded(int id,  QString URl, PageLoader::Status status);

    void finished();
private:
    QNetworkRequest m_request;
    QString m_textToFind;
    int m_depth;
    int m_id;
    QByteArray m_body;
//    QNetworkAccessManager m_netwManager;

private:
//    int getPage();
};

#endif // PAGELOADER_H
