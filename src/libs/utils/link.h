#pragma once

#include <utils/fileutils.h>

#include <QString>
#include <qmetatype.h>

#include <functional>


namespace Utils {

class QTCREATOR_UTILS_EXPORT Link
{
public:
    Link(const Utils::FilePath &filePath = Utils::FilePath(), int line = 0, int column = 0)
        : targetFilePath(filePath)
        , targetLine(line)
        , targetColumn(column)
    {}

    static Link fromString(const QString &fileName,
                           bool canContainLineNumber = false,
                           QString *postfix = nullptr);

    bool hasValidTarget() const
    { return !targetFilePath.isEmpty(); }

    bool hasValidLinkText() const
    { return linkTextStart != linkTextEnd; }

    bool operator==(const Link &other) const
    {
        return targetFilePath == other.targetFilePath
                && targetLine == other.targetLine
                && targetColumn == other.targetColumn
                && linkTextStart == other.linkTextStart
                && linkTextEnd == other.linkTextEnd;
    }
    bool operator!=(const Link &other) const { return !(*this == other); }

    int linkTextStart = -1;
    int linkTextEnd = -1;

    Utils::FilePath targetFilePath;
    int targetLine;
    int targetColumn;
};

uint QTCREATOR_UTILS_EXPORT qHash(const Link &l);

using ProcessLinkCallback = std::function<void(const Link &)>;

} // namespace Utils

Q_DECLARE_METATYPE(Utils::Link)
