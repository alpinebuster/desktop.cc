#pragma once

#include "utils_global.h"

#include <QVector>

QT_BEGIN_NAMESPACE
class QRegularExpression;
class QRegularExpressionMatch;
class QString;
QT_END_NAMESPACE

class QTCREATOR_UTILS_EXPORT FuzzyMatcher
{
public:
    enum class CaseSensitivity {
        CaseInsensitive,
        CaseSensitive,
        FirstLetterCaseSensitive
    };

    class HighlightingPositions {
    public:
        QVector<int> starts;
        QVector<int> lengths;
    };

    static QRegularExpression createRegExp(const QString &pattern,
            CaseSensitivity caseSensitivity = CaseSensitivity::CaseInsensitive);
    static QRegularExpression createRegExp(const QString &pattern,
                                           Qt::CaseSensitivity caseSensitivity);
    static HighlightingPositions highlightingPositions(const QRegularExpressionMatch &match);
};
