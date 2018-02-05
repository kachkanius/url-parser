#ifndef PAGELOADER_H
#define PAGELOADER_H

#include <PageParser.h>
#include <QRunnable>
#include <QObject>
#include <QUrl>

class PageLoader : public QObject, public QRunnable
{
    Q_OBJECT
public:
    PageLoader(const QString& sUrl, int depth);
    ~PageLoader();
    void run();
signals:
    void finished(QStringList urls, int depth);

private:
    QUrl m_url;
    QByteArray m_body;
    int m_depth;
    int getPage();
};

#endif // PAGELOADER_H
