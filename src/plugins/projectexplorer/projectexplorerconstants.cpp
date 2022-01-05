#include "projectexplorerconstants.h"

#include <coreplugin/icore.h>

#include <QCoreApplication>
#include <QString>

namespace ProjectExplorer {
namespace Constants {

QString msgAutoDetected()
{
    return QCoreApplication::translate("ProjectExplorer", "Auto-detected");
}

QString msgAutoDetectedToolTip()
{
    return QCoreApplication::translate("ProjectExplorer",
                                       "Automatically managed by %1 or the installer.")
        .arg(Core::ICore::ideDisplayName());
}

QString msgManual()
{
    return QCoreApplication::translate("ProjectExplorer", "Manual");
}

} // namespace Constants
} // namespace ProjectExplorer
