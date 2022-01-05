#pragma once

#include "core_global.h"

QT_BEGIN_NAMESPACE
class QString;
class QWidget;
QT_END_NAMESPACE

namespace Core {
namespace AsynchronousMessageBox {

CORE_EXPORT QWidget *warning(const QString &title, const QString &desciption);
CORE_EXPORT QWidget *information(const QString &title, const QString &desciption);
CORE_EXPORT QWidget *critical(const QString &title, const QString &desciption);

}
}
