#pragma once

#include <coreplugin/homemanager/ieditorfactory.h>

namespace ImageViewer {
namespace Internal {

class ImageViewerFactory final : public Core::IEditorFactory
{
public:
    ImageViewerFactory();
};

} // namespace Internal
} // namespace ImageViewer
