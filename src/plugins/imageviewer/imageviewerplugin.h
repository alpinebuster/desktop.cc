#pragma once

#include <extensionsystem/iplugin.h>

namespace ImageViewer {
namespace Internal {

class ImageViewerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.milab.Qt.MFDSPlugin" FILE "ImageViewer.json")

public:
    ImageViewerPlugin() = default;
    ~ImageViewerPlugin();

private:
    bool initialize(const QStringList &arguments, QString *errorMessage) final;

    class ImageViewerPluginPrivate *d = nullptr;
};

} // namespace Internal
} // namespace ImageViewer
