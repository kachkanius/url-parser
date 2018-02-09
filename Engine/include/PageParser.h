#ifndef PAGEPARSER_H
#define PAGEPARSER_H

#include <QString>
#include <QStringList>

class PageParser
{
public:
    PageParser(const QString& html);
    QStringList getUrls();
    int findStr(const QString& str);
    bool  containsStr(const QString& str, bool caseSensitive);
private:
    QString m_html;
};

#endif // PAGEPARSER_H
