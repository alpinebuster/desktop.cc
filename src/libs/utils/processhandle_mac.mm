#include "processhandle.h"
#import <AppKit/AppKit.h>

namespace Utils {

bool ProcessHandle::activate()
{
    NSRunningApplication *app = [NSRunningApplication
            runningApplicationWithProcessIdentifier:pid()];
    return app && [app activateWithOptions:static_cast<NSApplicationActivationOptions>(
            NSApplicationActivateIgnoringOtherApps)];
}

} // Utils
