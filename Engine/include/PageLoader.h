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
        HTTP_ERROR = 3,
        HTTP_NO_TEXT = 4
    };
    Q_OBJECT
public:
    PageLoader(const QString& sUrl, const QString& text, int depth);
    ~PageLoader();

    QString getUrl() const;
    int getId() const;
    void setId(int newId);

public slots:
    void start();

private slots:
    void requestEnd(QNetworkReply* reply);
    void networkError(QNetworkReply::NetworkError);

signals:
    void pageLoaded(int id, PageLoader::Status status, QStringList urls, int depth);
private:
    QNetworkRequest m_request;
    QString m_textToFind;
    int m_depth;
    int m_id;
    QNetworkAccessManager m_netwManager;
};

#endif // PAGELOADER_H
