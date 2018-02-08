#include "PageParser.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <iostream>
#include <QDebug>

static const QString A_TAG_PATTERN("(?i)<a([^>]+)>(.+?)</a>");
static const QString HREF_TAG_PATTERN( "\\s*(?i)href\\s*=\\s*\"(http*(([^\"]*)|'[^']*'|([^'>\\s]+)))");

PageParser::PageParser(const QString &html)
    : m_html(html)
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
        QRegularExpression link_exp(HREF_TAG_PATTERN);
        QRegularExpressionMatch link_match = link_exp.match(href_tag);
        if (link_match.hasMatch()) {
            QString link_str = link_match.captured(1);
            links_list.append(link_str);
        }
    }
    return links_list;
}

size_t PageParser::findStr(const QString &str)
{
    return m_html.count(str, Qt::CaseInsensitive);
}

bool PageParser::containsStr(const QString &str)
{
    return m_html.contains(str, Qt::CaseInsensitive);
}
