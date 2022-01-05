#pragma once

#include "namevaluemodel.h"

namespace Utils {

class QTCREATOR_UTILS_EXPORT EnvironmentModel : public NameValueModel
{
    Q_OBJECT

public:
    const Environment &baseEnvironment() const;
    void setBaseEnvironment(const Environment &env);
};

} // namespace Utils
