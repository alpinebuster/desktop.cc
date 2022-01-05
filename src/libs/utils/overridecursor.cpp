#include "overridecursor.h"

#include <QApplication>

using namespace Utils;

OverrideCursor::OverrideCursor(const QCursor &cursor)
    : m_cursor(cursor)
{
    QApplication::setOverrideCursor(cursor);
}

OverrideCursor::~OverrideCursor()
{
    if (m_set)
        QApplication::restoreOverrideCursor();
}

void OverrideCursor::set()
{
    if (!m_set) {
        QApplication::setOverrideCursor(m_cursor);
        m_set = true;
    }
}

void OverrideCursor::reset()
{
    if (m_set) {
        QApplication::restoreOverrideCursor();
        m_set = false;
    }
}

