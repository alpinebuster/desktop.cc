#pragma once

#include "utils_global.h"

#include <QCursor>

namespace Utils {

class QTCREATOR_UTILS_EXPORT OverrideCursor
{
public:
    OverrideCursor(const QCursor &cursor);
    ~OverrideCursor();
    void set();
    void reset();
private:
    bool m_set = true;
    QCursor m_cursor;
};

}
