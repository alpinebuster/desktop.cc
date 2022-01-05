#pragma once

#include <qglobal.h>

#if defined(QTSUPPORT_LIBRARY)
#  define QTSUPPORT_EXPORT Q_DECL_EXPORT
#else
#  define QTSUPPORT_EXPORT Q_DECL_IMPORT
#endif
