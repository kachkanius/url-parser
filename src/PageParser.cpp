#include "PageParser.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <iostream>
#include <QDebug>

static const QString A_TAG_PATTERN("(?i)<a([^>]+)>(.+?)</a>");
static const QString HREF_TAG_PATTERN( "\\s*(?i)href\\s*=\\s*\"(([^\"]*)|'[^']*'|([^'>\\s]+))");

PageParser::PageParser(const QString &html): m_html(html)
{
}

QStringList PageParser::getUrls()
{

    QStringList links_list;
    QRegularExpression atag_regex(A_TAG_PATTERN, QRegularExpression::PatternOption::CaseInsensitiveOption);
    QRegularExpressionMatchIterator iter = atag_regex.globalMatch(m_html);
    while (iter.hasNext()) {
        QRegularExpressionMatch href_match = iter.next();
        QString href_tag = href_match.captured(0);
        qDebug()<< "a tag: " << href_tag;

        QRegularExpression link_exp(HREF_TAG_PATTERN);
        QRegularExpressionMatch link_match = link_exp.match(href_tag);
        if (link_match.hasMatch()) {
            QString link_str = link_match.captured(1);
            qDebug() << "link: " << link_str;
            links_list.append(link_str);
        }
        qDebug() << "\n";
    }
    return links_list;

}
