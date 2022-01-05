#pragma once

#include "homeview.h"

#include <QPointer>

namespace Core {

class IContext;

namespace Internal {

class HomeArea : public SplitterOrView
{
    Q_OBJECT

public:
    HomeArea();
    ~HomeArea() override;

    IDocument *currentDocument() const;

signals:
    void windowTitleNeedsUpdate();

private:
    void focusChanged(QWidget *old, QWidget *now);
    void setCurrentView(HomeView *view);
    void updateCurrentEditor(IEditor *editor);
    void updateCloseSplitButton();

    IContext *m_context;
    QPointer<HomeView> m_currentView;
    QPointer<IDocument> m_currentDocument;
};

} // Internal
} // Core
