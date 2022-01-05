#pragma once

#include <qglobal.h>

namespace ImageViewer {
namespace Constants {

const char IMAGEVIEWER_ID[] = "Editors.ImageViewer";
const char IMAGEVIEWER_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("OpenWith::Editors", "Image Viewer");

const char ACTION_EXPORT_IMAGE[] = "ImageViewer.ExportImage";
const char ACTION_EXPORT_MULTI_IMAGES[] = "ImageViewer.ExportMultiImages";
const char ACTION_FIT_TO_SCREEN[] = "ImageViewer.FitToScreen";
const char ACTION_BACKGROUND[] = "ImageViewer.Background";
const char ACTION_OUTLINE[] = "ImageViewer.Outline";
const char ACTION_TOGGLE_ANIMATION[] = "ImageViewer.ToggleAnimation";

} // namespace Constants
} // namespace ImageViewer
