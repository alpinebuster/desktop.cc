#pragma once

#include "utils_global.h"
#include "fileutils.h"

#include <QLabel>

namespace Utils {

class QTCREATOR_UTILS_EXPORT FileCrumbLabel : public QLabel
{
    Q_OBJECT
public:
    FileCrumbLabel(QWidget *parent = nullptr);

    void setPath(const FilePath &path);

signals:
    void pathClicked(const FilePath &path);
};

} // Utils
