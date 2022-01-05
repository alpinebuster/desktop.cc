#pragma once

#include <utils/id.h>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

// Run the settings dialog and wait for it to finish.
// Returns if the changes have been applied.
bool executeSettingsDialog(QWidget *parent, Utils::Id initialPage);

} // namespace Internal
} // namespace Core
