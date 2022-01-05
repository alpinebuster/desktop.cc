#pragma once

#include "utils_global.h"

#include "optional.h"

#include <QMetaType>

namespace Utils {

class QTCREATOR_UTILS_EXPORT LineColumn
{
public:
    constexpr LineColumn() = default;
    constexpr
    LineColumn(int line, int column)
        : line(line),
        column(column)
    {}


    bool isValid() const
    {
        return line >= 0 && column >= 0;
    }

    friend bool operator==(LineColumn first, LineColumn second)
    {
        return first.isValid() && first.line == second.line && first.column == second.column;
    }

    friend bool operator!=(LineColumn first, LineColumn second)
    {
        return !(first == second);
    }

    static LineColumn extractFromFileName(const QString &fileName, int &postfixPos);

public:
    int line = -1;
    int column = -1;
};

using OptionalLineColumn = optional<LineColumn>;

} // namespace Utils

Q_DECLARE_METATYPE(Utils::LineColumn)
