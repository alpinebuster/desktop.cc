#pragma once

#include "utils_global.h"
#include "macroexpander.h"

#include <QWidget>

#include <functional>

namespace Utils {

namespace Internal { class VariableChooserPrivate; }

class QTCREATOR_UTILS_EXPORT VariableChooser : public QWidget
{
    Q_OBJECT

public:
    explicit VariableChooser(QWidget *parent = nullptr);
    ~VariableChooser() override;

    void addMacroExpanderProvider(const Utils::MacroExpanderProvider &provider);
    void addSupportedWidget(QWidget *textcontrol, const QByteArray &ownName = QByteArray());

    static void addSupportForChildWidgets(QWidget *parent, Utils::MacroExpander *expander);

protected:
    bool event(QEvent *ev) override;
    bool eventFilter(QObject *, QEvent *event) override;

private:
    Internal::VariableChooserPrivate *d;
};

} // namespace Utils
