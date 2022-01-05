#pragma once

#include <coreplugin/iwelcomepage.h>

QT_BEGIN_NAMESPACE
class QFileInfo;
QT_END_NAMESPACE

namespace Support {
namespace Internal {

class ExampleItem;

class ExamplesWelcomePage : public Core::IWelcomePage
{
    Q_OBJECT

public:
    explicit ExamplesWelcomePage(bool showExamples);

    QString title() const final;
    int priority() const final;
    Utils::Id id() const final;
    QWidget *createWidget() const final;

    static void openProject(const ExampleItem *item);

private:
    static QString copyToAlternativeLocation(const QFileInfo &fileInfo, QStringList &filesToOpen, const QStringList &dependencies);
    const bool m_showExamples;
};

} // namespace Internal
} // namespace QtSupport
