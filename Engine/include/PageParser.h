#ifndef PAGEPARSER_H
#define PAGEPARSER_H

#include <QString>
#include <QStringList>

class PageParser
{
public:
    PageParser(const QString& html);
    QStringList getUrls();
private:
    QString m_html;
};

#endif // PAGEPARSER_H
