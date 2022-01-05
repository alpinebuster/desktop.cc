#include "imageviewerplugin.h"
#include "imageviewer.h"
#include "imageviewerfactory.h"
#include "imageviewerconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/homemanager/homemanager.h>

#include <QAction>
#include <QCoreApplication>
#include <QKeySequence>

using namespace Core;
using namespace Utils;

namespace ImageViewer {
namespace Internal {

class ImageViewerAction final : public QAction
{
public:
    ImageViewerAction(Id id,
                      const std::function<void(ImageViewer *v)> &onTriggered,
                      const QString &title = {},
                      const QKeySequence &key = {})
        : QAction(title)
    {
        Command *command = ActionManager::registerAction(this, id, Context(Constants::IMAGEVIEWER_ID));
        if (!key.isEmpty())
            command->setDefaultKeySequence(key);

        connect(this, &QAction::triggered, this, [onTriggered] {
            if (auto iv = qobject_cast<ImageViewer *>(HomeManager::currentEditor()))
                onTriggered(iv);
        });
    }
};

class ImageViewerPluginPrivate final
{
    Q_DECLARE_TR_FUNCTIONS(Imageviewer::Internal::ImageViewerPlugin)

public:
    ImageViewerFactory imageViewerFactory;

    ImageViewerAction zoomInAction {
        Core::Constants::ZOOM_IN,
        &ImageViewer::zoomIn
    };

    ImageViewerAction zoomOutAction {
        Core::Constants::ZOOM_OUT,
        &ImageViewer::zoomOut
    };

    ImageViewerAction zoomResetAction {
        Core::Constants::ZOOM_RESET,
        &ImageViewer::resetToOriginalSize
    };

    ImageViewerAction fitToScreenAction {
        Constants::ACTION_FIT_TO_SCREEN,
        &ImageViewer::fitToScreen,
        tr("Fit to Screen"),
        tr("Ctrl+=")
    };

    ImageViewerAction switchBackgroundAction {
        Constants::ACTION_BACKGROUND,
        &ImageViewer::switchViewBackground,
        tr("Switch Background"),
        tr("Ctrl+[")
    };

    ImageViewerAction switchOutlineAction {
        Constants::ACTION_OUTLINE,
        &ImageViewer::switchViewOutline,
        tr("Switch Outline"),
        tr("Ctrl+]")
    };

    ImageViewerAction toggleAnimationAction {
        Constants::ACTION_TOGGLE_ANIMATION,
        &ImageViewer::togglePlay,
        tr("Toggle Animation")
    };

    ImageViewerAction exportImageAction {
        Constants::ACTION_EXPORT_IMAGE,
        &ImageViewer::exportImage,
        tr("Export Image")
    };

    ImageViewerAction exportMulitImagesAction {
        Constants::ACTION_EXPORT_MULTI_IMAGES,
        &ImageViewer::exportMultiImages,
        tr("Export Multiple Images"),
    };
};

ImageViewerPlugin::~ImageViewerPlugin()
{
    delete d;
}

bool ImageViewerPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    d = new ImageViewerPluginPrivate;

    return true;
}

} // namespace Internal
} // namespace ImageViewer
