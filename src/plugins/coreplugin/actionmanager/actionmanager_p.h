#pragma once

#include <coreplugin/actionmanager/command_p.h>
#include <coreplugin/actionmanager/actioncontainer_p.h>
#include <coreplugin/icontext.h>

#include <QMap>
#include <QHash>
#include <QMultiHash>
#include <QTimer>

namespace Core {

namespace Internal {

class Action;
class ActionContainerPrivate;

class ActionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    using IdCmdMap = QHash<Utils::Id, Action *>;
    using IdContainerMap = QHash<Utils::Id, ActionContainerPrivate *>;

    ~ActionManagerPrivate() override;

    void setContext(const Context &context);
    bool hasContext(int context) const;

    void saveSettings();
    static void saveSettings(Action *cmd);

    static void showShortcutPopup(const QString &shortcut);
    bool hasContext(const Context &context) const;
    Action *overridableAction(Utils::Id id);

    static void readUserSettings(Utils::Id id, Action *cmd);

    void containerDestroyed();
    void actionTriggered();

    IdCmdMap m_idCmdMap;

    IdContainerMap m_idContainerMap;

    Context m_context;

    bool m_presentationModeEnabled = false;
};

} // namespace Internal
} // namespace Core
