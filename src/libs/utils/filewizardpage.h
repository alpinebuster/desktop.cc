#pragma once

#include "utils_global.h"

#include "wizardpage.h"

namespace Utils {

class FileWizardPagePrivate;

class QTCREATOR_UTILS_EXPORT FileWizardPage : public WizardPage
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath DESIGNABLE true)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName DESIGNABLE true)

public:
    explicit FileWizardPage(QWidget *parent = nullptr);
    ~FileWizardPage() override;

    QString fileName() const;
    QString path() const;

    bool isComplete() const override;

    void setFileNameLabel(const QString &label);
    void setPathLabel(const QString &label);
    void setDefaultSuffix(const QString &suffix);

    bool forceFirstCapitalLetterForFileName() const;
    void setForceFirstCapitalLetterForFileName(bool b);

    // Validate a base name entry field (potentially containing extension)
    static bool validateBaseName(const QString &name, QString *errorMessage = nullptr);

signals:
    void activated();
    void pathChanged();

public slots:
    void setPath(const QString &path);
    void setFileName(const QString &name);

private:
    void slotValidChanged();
    void slotActivated();

    FileWizardPagePrivate *d;
};

} // namespace Utils
