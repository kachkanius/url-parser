#ifndef PAGELOADER_H
#define PAGELOADER_H

#include <PageParser.h>
#include <QRunnable>
#include <QObject>
#include <QUrl>

class PageLoader : public QObject, public QRunnable
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

    void run();
signals:
    void pageLoaded(PageLoader::Status status, QStringList urls, size_t id, int depth);

private:
    QUrl m_url;
    int m_depth;
    QString m_str;
    size_t m_id;

    QByteArray m_body;
private:
    int getPage();
};

#endif // PAGELOADER_H
