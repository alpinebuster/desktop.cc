#pragma once

#include "iplugin.h"

namespace ExtensionSystem {

class PluginSpec;

namespace Internal {

class IPluginPrivate
{
public:
    PluginSpec *pluginSpec;
};

} // namespace Internal
} // namespace ExtensionSystem
