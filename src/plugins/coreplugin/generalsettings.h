#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

namespace Core {
namespace Internal {

class GeneralSettings : public IOptionsPage
{
public:
    GeneralSettings();

    static bool showShortcutsInContextMenu();
    void setShowShortcutsInContextMenu(bool show);

private:
    friend class GeneralSettingsWidget;
    bool m_defaultShowShortcutsInContextMenu;
};

} // namespace Internal
} // namespace Core
