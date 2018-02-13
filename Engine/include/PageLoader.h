#ifndef PAGELOADER_H
#define PAGELOADER_H

#include <PageParser.h>
#include <QObject>
#include <QUrl>
#include <QNetworkReply>

class PageLoader : public QObject
{
public:
    enum class Status: int {
        LOADING = 0, // Downloading in process.
        FOUND = 1, // Serching text fount
        NOT_FOUND = 2, // Searching text was not found
        HTTP_ERROR = 3, // Some error occurs
        HTTP_NO_TEXT = 4 // Page did not contains any text
    };
    Q_ENUM(Status)

    Q_OBJECT
public:
    PageLoader(const QString& sUrl, const QString& text, bool caseSensitive, int depth);
    ~PageLoader();

    // geters/setters
    QString getUrl() const;
    int getId() const;
    void setId(int newId);

public slots:
    // Start/stop download
    void start();
    void stop();

signals:
    // Callback that downloading was done. (since QNetworkAccessManager works asynchronously)
    void pageLoaded(int id, PageLoader::Status status, QString err, QStringList urls, int depth);

    // Force stop (call abort() ) for request.
    void cancelDownloading();

private slots:
    // Callbacks to handle reply
    void requestEnd(QNetworkReply* reply);
    void networkError(QNetworkReply::NetworkError);

private:
    QNetworkRequest m_request;
    QNetworkAccessManager m_netwManager;
    QString m_textToFind;
    int m_depth;
    int m_id;
    bool m_isActive;
    bool m_caseSensitive;
};

#endif // PAGELOADER_H
