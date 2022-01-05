#pragma once

#include "core_global.h"

#include <utils/id.h>

#include <QSet>
#include <QStringList>

namespace Core {

class CORE_EXPORT FeatureSet;

class CORE_EXPORT IFeatureProvider
{
public:
    virtual ~IFeatureProvider() = default;

    virtual QSet<Utils::Id> availableFeatures(Utils::Id id) const = 0;
    virtual QSet<Utils::Id> availablePlatforms() const = 0;
    virtual QString displayNameForPlatform(Utils::Id id) const = 0;
};

} // namespace Core
