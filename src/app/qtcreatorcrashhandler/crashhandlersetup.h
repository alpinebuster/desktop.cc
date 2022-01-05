#pragma once

#include <QString>

class CrashHandlerSetup
{
public:
    enum RestartCapability { EnableRestart, DisableRestart };

    CrashHandlerSetup(const QString &appName,
                      RestartCapability restartCap = EnableRestart,
                      const QString &executableDirPath = QString());
    ~CrashHandlerSetup();
};
