#pragma once

#include "utils_global.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QWidget;
QT_END_NAMESPACE

namespace Utils {

QTCREATOR_UTILS_EXPORT QAction *execMenuAtWidget(QMenu *menu, QWidget *widget);

} // namespace Utils
