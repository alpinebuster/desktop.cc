#pragma once

#include <QCoreApplication>

namespace Core {
namespace Internal {

class PluginInstallWizard
{
    Q_DECLARE_TR_FUNCTIONS(Core::Internal::PluginInstallWizard)

public:
    static bool exec();
};

} // namespace Internal
} // namespace Core
