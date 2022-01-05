#pragma once

#include "core_global.h"

#include "generatedfile.h"

#include <utils/wizard.h>

#include <QVariantMap>

namespace Core {

class BaseFileWizardFactory;

class CORE_EXPORT BaseFileWizard : public Utils::Wizard
{
    Q_OBJECT

public:
    explicit BaseFileWizard(const BaseFileWizardFactory *factory, const QVariantMap &extraValues,
                            QWidget *parent = nullptr);

    void initializePage(int id) override;

    QList<QWizardPage *> extensionPages();

    void accept() override;
    void reject() override;

private:
    void generateFileList();

    QVariantMap m_extraValues;
    const BaseFileWizardFactory *m_factory;
    QList<QWizardPage *> m_extensionPages;
    QWizardPage *m_firstExtensionPage = nullptr;
    GeneratedFiles m_files;
};

} // namespace Core
