#pragma once

#include <coreplugin/core_global.h>

#include <QStyle>
#include <QFileIconProvider>

namespace Utils { class FilePath; }

namespace Core {

namespace FileIconProvider {

// Access to the single instance
CORE_EXPORT QFileIconProvider *iconProvider();

// Access to individual items
CORE_EXPORT QIcon icon(const Utils::FilePath &filePath);
CORE_EXPORT QIcon icon(QFileIconProvider::IconType type);

// Register additional overlay icons
CORE_EXPORT QPixmap overlayIcon(const QPixmap &baseIcon, const QIcon &overlayIcon);
CORE_EXPORT QPixmap overlayIcon(QStyle::StandardPixmap baseIcon, const QIcon &overlayIcon, const QSize &size);
CORE_EXPORT void registerIconOverlayForSuffix(const QString &path, const QString &suffix);
CORE_EXPORT void registerIconOverlayForFilename(const QString &path, const QString &filename);
CORE_EXPORT void registerIconOverlayForMimeType(const QString &path, const QString &mimeType);
CORE_EXPORT void registerIconOverlayForMimeType(const QIcon &icon, const QString &mimeType);

CORE_EXPORT QIcon directoryIcon(const QString &overlay);

} // namespace FileIconProvider
} // namespace Core
