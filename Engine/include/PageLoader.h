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
    PageLoader(const QString& sUrl);
    ~PageLoader();
    void run();
signals:
    void finished(QStringList urls);

private:
    QUrl m_url;
    QByteArray m_body;
    int getPage();
};

#endif // PAGELOADER_H
