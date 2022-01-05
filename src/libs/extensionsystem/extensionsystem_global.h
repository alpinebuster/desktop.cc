#pragma once

#include <QLoggingCategory>
#include <qglobal.h>

#if defined(EXTENSIONSYSTEM_LIBRARY)
#  define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#  define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

Q_DECLARE_LOGGING_CATEGORY(pluginLog)
