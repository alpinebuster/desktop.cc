#pragma once

#include "utils_global.h"

#include <QTextCharFormat>

namespace Utils {

class QTCREATOR_UTILS_EXPORT FormattedText {
public:
    FormattedText() = default;
    FormattedText(const FormattedText &other) = default;
    FormattedText(const QString &txt, const QTextCharFormat &fmt = QTextCharFormat()) :
        text(txt), format(fmt)
    { }

    QString text;
    QTextCharFormat format;
};

class QTCREATOR_UTILS_EXPORT AnsiEscapeCodeHandler
{
public:
    QList<FormattedText> parseText(const FormattedText &input);
    void endFormatScope();

private:
    void setFormatScope(const QTextCharFormat &charFormat);

    bool            m_previousFormatClosed = true;
    bool            m_waitingForTerminator = false;
    QString         m_alternateTerminator;
    QTextCharFormat m_previousFormat;
    QString         m_pendingText;
};

} // namespace Utils
