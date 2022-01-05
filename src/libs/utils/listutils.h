#pragma once

#include <QList>

namespace Utils {

template <class T1, class T2>
QList<T1> qwConvertList(const QList<T2> &list)
{
    QList<T1> convertedList;
    for (T2 listEntry : list) {
        convertedList << qobject_cast<T1>(listEntry);
    }
    return convertedList;
}

} // namespace Utils
