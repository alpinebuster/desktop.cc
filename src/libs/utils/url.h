#pragma once

#include "utils_global.h"
#include <QUrl>

namespace Utils {

QTCREATOR_UTILS_EXPORT QUrl urlFromLocalHostAndFreePort();
QTCREATOR_UTILS_EXPORT QUrl urlFromLocalSocket();
QTCREATOR_UTILS_EXPORT QString urlSocketScheme();
QTCREATOR_UTILS_EXPORT QString urlTcpScheme();

}
