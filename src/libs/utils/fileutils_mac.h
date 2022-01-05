#pragma once

#include <QUrl>

namespace Utils {
namespace Internal {

QUrl filePathUrl(const QUrl &url);
QString normalizePathName(const QString &filePath);

} // Internal
} // Utils
