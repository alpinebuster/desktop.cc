#ifndef QTCREATOR_PCH_H
#define QTCREATOR_PCH_H

#if defined __cplusplus
#include <QtGlobal>

#ifdef Q_OS_WIN
#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

// lib/Utils needs defines for Windows 8
#undef WINVER
#define WINVER 0x0602
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0602

#define NOHELP
#include <qt_windows.h>

#undef DELETE
#undef IN
#undef OUT
#undef ERROR
#undef ABSOLUTE

// LLVM 12 comes with CALLBACK as a template argument
#undef CALLBACK

#define _POSIX_
#include <limits.h>
#undef _POSIX_
#endif // Q_OS_WIN

#include <QCoreApplication>
#include <QList>
#include <QVariant>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QDebug>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QTextStream>
using Qt::endl;
using Qt::forcesign;
using Qt::noshowbase;
using Qt::dec;
using Qt::showbase;
using Qt::hex;
using Qt::noforcesign;
#else
#include <QTextCodec>
#endif

#include <stdlib.h>
#endif

#endif // QTCREATOR_PCH_H
