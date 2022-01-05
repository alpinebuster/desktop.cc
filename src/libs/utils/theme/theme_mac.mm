#include "theme_mac.h"

#include <qglobal.h>
#include <QOperatingSystemVersion>

#include <AppKit/AppKit.h>

#if !QT_MACOS_PLATFORM_SDK_EQUAL_OR_ABOVE(__MAC_10_14)
@interface NSApplication (MojaveForwardDeclarations)
@property (strong) NSAppearance *appearance NS_AVAILABLE_MAC(10_14);
@end
#endif

namespace Utils {
namespace Internal {

void forceMacOSLightAquaApperance()
{
#if __has_builtin(__builtin_available)
    if (__builtin_available(macOS 10.14, *))
#else // Xcode 8
    if (QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 14, 0))
#endif
        NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameAqua];
}

} // Internal
} // Utils
