#pragma once

#include <QMap>
#include <QRect>
#include <QImage>

namespace Support {
namespace Internal {

class ScreenshotCropper
{
public:
    static QImage croppedImage(const QImage &sourceImage, const QString &filePath, const QSize &cropSize);
    static QMap<QString, QRect> loadAreasOfInterest(const QString &areasXmlFile);
    static bool saveAreasOfInterest(const QString &areasXmlFile, QMap<QString, QRect> &areas);
};

} // namespace Internal
} // namespace QtSupport
