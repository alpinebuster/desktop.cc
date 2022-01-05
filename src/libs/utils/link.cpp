#include "link.h"

#include "linecolumn.h"

namespace Utils {

/*!
    Returns the Link to \a fileName.
    If \a canContainLineNumber is true the line number, and column number components
    are extracted from \a fileName and the found \a postfix is set.

    The following patterns are supported: \c {filepath.txt:19},
    \c{filepath.txt:19:12}, \c {filepath.txt+19},
    \c {filepath.txt+19+12}, and \c {filepath.txt(19)}.
*/
Link Link::fromString(const QString &fileName, bool canContainLineNumber, QString *postfix)
{
    if (!canContainLineNumber)
        return {Utils::FilePath::fromString(fileName)};
    int postfixPos = -1;
    const LineColumn lineColumn = LineColumn::extractFromFileName(fileName, postfixPos);
    if (postfix && postfixPos >= 0)
        *postfix = fileName.mid(postfixPos);
    return {Utils::FilePath::fromString(fileName.left(postfixPos)),
            lineColumn.line,
            lineColumn.column};
}

uint qHash(const Link &l)
{
    QString s = l.targetFilePath.toString();
    return qHash(s.append(':').append(QString::number(l.targetLine)).append(':')
                 .append(QString::number(l.targetColumn)));
}

} // namespace Utils
