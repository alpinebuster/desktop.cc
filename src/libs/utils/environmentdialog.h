#pragma once

#include "environment.h"
#include "namevaluesdialog.h"
#include <thread>

namespace Utils {

class QTCREATOR_UTILS_EXPORT EnvironmentDialog : public NameValuesDialog
{
    Q_OBJECT
public:
    static Utils::optional<EnvironmentItems> getEnvironmentItems(QWidget *parent = nullptr,
                                                                 const EnvironmentItems &initial = {},
                                                                 const QString &placeholderText = {},
                                                                 Polisher polish = {});
};

} // namespace Utils
