#pragma once

#include <extensionsystem/iplugin.h>

namespace Support {
namespace Internal {

class SupportPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.milab.Qt.MFDSPlugin" FILE "Support.json")

public:
    ~SupportPlugin() final;

private:
    bool initialize(const QStringList &arguments, QString *errorMessage) final;
    void extensionsInitialized() final;

    class SupportPluginPrivate *d = nullptr;
};

} // namespace Internal
} // namespace QtSupport
