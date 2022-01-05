#pragma once

#include <coreplugin/homemanager/ieditor.h>
#include <coreplugin/idocument.h>

#include <QScopedPointer>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAction;
QT_END_NAMESPACE

namespace ImageViewer {
namespace Internal {
class ImageViewerFile;

class ImageViewer : public Core::IEditor
{
    Q_OBJECT

public:
    ImageViewer();
    ~ImageViewer() override;

    Core::IDocument *document() const override;
    QWidget *toolBar() override;

    IEditor *duplicate() override;

    void exportImage();
    void exportMultiImages();
    void imageSizeUpdated(const QSize &size);
    void scaleFactorUpdate(qreal factor);

    void switchViewBackground();
    void switchViewOutline();
    void zoomIn();
    void zoomOut();
    void resetToOriginalSize();
    void fitToScreen();
    void updateToolButtons();
    void togglePlay();

private:
    ImageViewer(const QSharedPointer<ImageViewerFile> &document);
    void ctor();
    void playToggled();
    void updatePauseAction();

    struct ImageViewerPrivate *d;
};

} // namespace Internal
} // namespace ImageViewer
