#pragma once

#include "extensionsystem_global.h"

#include <QWidget>

namespace ExtensionSystem {

class PluginSpec;
namespace Internal {
namespace Ui { class PluginErrorView; }
} // namespace Internal

class EXTENSIONSYSTEM_EXPORT PluginErrorView : public QWidget
{
    Q_OBJECT

public:
    PluginErrorView(QWidget *parent = nullptr);
    ~PluginErrorView() override;

    void update(PluginSpec *spec);

private:
    Internal::Ui::PluginErrorView *m_ui;
};

} // namespace ExtensionSystem
