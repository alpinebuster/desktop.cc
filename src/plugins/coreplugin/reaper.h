#pragma once

#include "core_global.h"

namespace Utils { class QtcProcess; }

namespace Core {
namespace Reaper {

CORE_EXPORT void reap(Utils::QtcProcess *p, int timeoutMs = 500);

} // namespace Reaper
} // namespace Core
