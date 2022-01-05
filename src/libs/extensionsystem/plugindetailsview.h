#pragma once

#include "extensionsystem_global.h"

#include <QWidget>

namespace ExtensionSystem {

class PluginSpec;

namespace Internal {
namespace Ui { class PluginDetailsView; }
} // namespace Internal


class EXTENSIONSYSTEM_EXPORT PluginDetailsView : public QWidget
{
    Q_OBJECT

public:
    PluginDetailsView(QWidget *parent = nullptr);
    ~PluginDetailsView() override;

    void update(PluginSpec *spec);

private:
    Internal::Ui::PluginDetailsView *m_ui;
};

} // namespace ExtensionSystem
