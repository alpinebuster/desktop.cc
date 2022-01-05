#include "imageviewerfactory.h"
#include "imageviewerconstants.h"
#include "imageviewer.h"

#include <QCoreApplication>
#include <QImageReader>

namespace ImageViewer {
namespace Internal {

ImageViewerFactory::ImageViewerFactory()
{
    setId(Constants::IMAGEVIEWER_ID);
    setDisplayName(QCoreApplication::translate("OpenWith::Editors", Constants::IMAGEVIEWER_DISPLAY_NAME));
    setEditorCreator([] { return new ImageViewer; });

    const QList<QByteArray> supportedMimeTypes = QImageReader::supportedMimeTypes();
    for (const QByteArray &format : supportedMimeTypes)
        addMimeType(QString::fromLatin1(format));
}

} // namespace Internal
} // namespace ImageViewer
