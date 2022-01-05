#pragma once

#include "extensionsystem_global.h"

#include <utils/treemodel.h>

#include <QWidget>

namespace Utils {
class CategorySortFilterModel;
class TreeView;
} // Utils

namespace ExtensionSystem {

class PluginSpec;

namespace Internal {
class CollectionItem;
class PluginItem;
} // Internal

class EXTENSIONSYSTEM_EXPORT PluginView : public QWidget
{
    Q_OBJECT

public:
    explicit PluginView(QWidget *parent = nullptr);
    ~PluginView() override;

    PluginSpec *currentPlugin() const;
    void setFilter(const QString &filter);

signals:
    void currentPluginChanged(ExtensionSystem::PluginSpec *spec);
    void pluginActivated(ExtensionSystem::PluginSpec *spec);
    void pluginSettingsChanged(ExtensionSystem::PluginSpec *spec);

private:
    PluginSpec *pluginForIndex(const QModelIndex &index) const;
    void updatePlugins();
    bool setPluginsEnabled(const QSet<PluginSpec *> &plugins, bool enable);

    Utils::TreeView *m_categoryView;
    Utils::TreeModel<Utils::TreeItem, Internal::CollectionItem, Internal::PluginItem> *m_model;
    Utils::CategorySortFilterModel *m_sortModel;

    friend class Internal::CollectionItem;
    friend class Internal::PluginItem;
};

} // namespae ExtensionSystem
