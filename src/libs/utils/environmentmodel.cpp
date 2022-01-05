#include "environmentmodel.h"

#include <utils/algorithm.h>
#include <utils/environment.h>
#include <utils/hostosinfo.h>

#include <QString>
#include <QFont>

namespace Utils {
const Environment &EnvironmentModel::baseEnvironment() const
{
    return static_cast<const Environment &>(baseNameValueDictionary());
}
void EnvironmentModel::setBaseEnvironment(const Environment &env)
{
    setBaseNameValueDictionary(env);
}
} // namespace Utils
