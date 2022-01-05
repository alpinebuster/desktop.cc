#pragma once

#include "utils_global.h"
#include "wizardpage.h"
#include "infolabel.h"

namespace Utils {

class ProjectIntroPagePrivate;

class QTCREATOR_UTILS_EXPORT ProjectIntroPage : public WizardPage
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description WRITE setDescription DESIGNABLE true)
    Q_PROPERTY(QString path READ path WRITE setPath DESIGNABLE true)
    Q_PROPERTY(QString projectName READ projectName WRITE setProjectName DESIGNABLE true)
    Q_PROPERTY(bool useAsDefaultPath READ useAsDefaultPath WRITE setUseAsDefaultPath DESIGNABLE true)
    Q_PROPERTY(bool forceSubProject READ forceSubProject WRITE setForceSubProject DESIGNABLE true)

public:
    explicit ProjectIntroPage(QWidget *parent = nullptr);
    ~ProjectIntroPage() override;

    QString projectName() const;
    QString path() const;
    QString description() const;
    bool useAsDefaultPath() const;

    // Insert an additional control into the form layout for the target.
    void insertControl(int row, QWidget *label, QWidget *control);

    bool isComplete() const override;

    bool forceSubProject() const;
    void setForceSubProject(bool force);
    void setProjectList(const QStringList &projectList);
    void setProjectDirectories(const QStringList &directoryList);
    int projectIndex() const;

    bool validateProjectName(const QString &name, QString *errorMessage);

signals:
    void activated();

public slots:
    void setPath(const QString &path);
    void setProjectName(const QString &name);
    void setDescription(const QString &description);
    void setUseAsDefaultPath(bool u);
    void setProjectNameRegularExpression(const QRegularExpression &regEx);

private:
    void slotChanged();
    void slotActivated();

    bool validate();
    void displayStatusMessage(InfoLabel::InfoType t, const QString &);
    void hideStatusLabel();

    ProjectIntroPagePrivate *d;
};

} // namespace Utils
