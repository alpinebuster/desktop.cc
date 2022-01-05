#pragma once

#include "utils_global.h"

#include <QTextCursor>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPlainTextEdit;
QT_END_NAMESPACE

class QTCREATOR_UTILS_EXPORT CamelCaseCursor
{
public:
    static bool left(QTextCursor *cursor, QPlainTextEdit *edit, QTextCursor::MoveMode mode);
    static bool left(QLineEdit *edit, QTextCursor::MoveMode mode);
    static bool right(QTextCursor *cursor, QPlainTextEdit *edit, QTextCursor::MoveMode mode);
    static bool right(QLineEdit *edit, QTextCursor::MoveMode mode);
};
