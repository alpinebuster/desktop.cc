#pragma once

#include "utils_global.h"

#include <QTemporaryFile>

namespace Utils {

class QTCREATOR_UTILS_EXPORT TemporaryFile : public QTemporaryFile
{
public:
    explicit TemporaryFile(const QString &pattern);
};

} // namespace Utils
