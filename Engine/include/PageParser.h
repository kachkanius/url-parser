#ifndef PAGEPARSER_H
#define PAGEPARSER_H

#include <QString>
#include <QStringList>

class PageParser
{
public:
    PageParser(const QString& html);
    QStringList getUrls();
    size_t findStr(const QString& str);
    bool  containsStr(const QString& str);
private:
    QString m_html;
};

#endif // PAGEPARSER_H
