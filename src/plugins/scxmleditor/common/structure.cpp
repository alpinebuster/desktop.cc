// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "actionhandler.h"
#include "actionprovider.h"
#include "graphicsscene.h"
#include "sceneutils.h"
#include "scxmldocument.h"
#include "scxmleditorconstants.h"
#include "scxmleditortr.h"
#include "scxmlnamespace.h"
#include "scxmltagutils.h"
#include "scxmluifactory.h"
#include "structure.h"
#include "structuremodel.h"
#include "treeview.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QToolBar>
#include <QToolButton>
#include <QUndoStack>

#include <utils/utilsicons.h>
#include <utils/qtcassert.h>

using namespace ScxmlEditor::PluginInterface;
using namespace ScxmlEditor::Common;

TreeItemDelegate::TreeItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *TreeItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        auto edit = new QLineEdit(parent);
        edit->setFocusPolicy(Qt::StrongFocus);
        static const QRegularExpression rx("^(?!xml)[_a-z][a-z0-9-._]*$",
                                           QRegularExpression::CaseInsensitiveOption);
        edit->setValidator(new QRegularExpressionValidator(rx, parent));
        return edit;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

StructureSortFilterProxyModel::StructureSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void StructureSortFilterProxyModel::setVisibleTags(const QList<TagType> &visibleTags)
{
    m_visibleTags = visibleTags;
    if (!m_visibleTags.contains(Scxml))
        m_visibleTags << Scxml;
    invalidateFilter();
}

void StructureSortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_sourceModel = static_cast<StructureModel*>(sourceModel);
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

bool StructureSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_sourceModel) {
        ScxmlTag *tag = m_sourceModel->getItem(source_parent, source_row);
        if (tag) {
            ScxmlNamespace *ns = tag->document()->scxmlNamespace(tag->prefix());
            bool nsBool = (!ns) || ns->isTagVisible(tag->tagName(false));
            return m_visibleTags.contains(tag->tagType()) && nsBool;
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

Structure::Structure(QWidget *parent)
    : QFrame(parent)
{
    createUi();

    addCheckbox(Tr::tr("Common states"), State);
    addCheckbox(Tr::tr("Metadata"), Metadata);
    addCheckbox(Tr::tr("Other tags"), OnEntry);
    addCheckbox(Tr::tr("Unknown tags"), UnknownTag);

    m_tagVisibilityFrame->setVisible(false);
    connect(m_checkboxButton, &QToolButton::toggled, m_tagVisibilityFrame, &QFrame::setVisible);

    m_model = new StructureModel(this);

    m_proxyModel = new StructureSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setDynamicSortFilter(false);

    // Default set of the visible tags
    QList<TagType> visibleTags;
    for (int i = 0; i < Finalize; ++i)
        visibleTags << TagType(i);
    m_proxyModel->setVisibleTags(visibleTags);

    m_structureView->setModel(m_proxyModel);
    m_structureView->setItemDelegate(new TreeItemDelegate(this));

    connect(m_structureView, &TreeView::pressed, this, &Structure::rowActivated);
    connect(m_structureView, &TreeView::rightButtonClicked, this, &Structure::showMenu);
    connect(m_structureView, &TreeView::entered, this, &Structure::rowEntered);

    connect(m_model, &StructureModel::selectIndex, this, &Structure::currentTagChanged);
    connect(m_model, &StructureModel::childAdded, this, &Structure::childAdded);
}

void Structure::addCheckbox(const QString &name, TagType type)
{
    auto box = new QCheckBox;
    box->setText(name);
    box->setProperty(Constants::C_SCXMLTAG_TAGTYPE, type);
    box->setCheckable(true);
    box->setChecked(true);
    connect(box, &QCheckBox::clicked, this, &Structure::updateCheckBoxes);
    m_checkboxFrame->layout()->addWidget(box);
    m_checkboxes << box;
}

void Structure::updateCheckBoxes()
{
    QList<TagType> visibleTags;
    for (QCheckBox *box : std::as_const(m_checkboxes)) {
        if (box->isChecked()) {
            switch (TagType(box->property(Constants::C_SCXMLTAG_TAGTYPE).toInt())) {
            case State:
                visibleTags << Initial << Final << History << State << Parallel << Transition << InitialTransition;
                break;
            case Metadata:
                visibleTags << Metadata << MetadataItem;
                break;
            case OnEntry:
                visibleTags << OnEntry << OnExit << Raise << If << ElseIf << Else
                            << Foreach << Log << DataModel << Data << Assign << Donedata
                            << Content << Param << Script << Send << Cancel << Invoke << Finalize;
                break;
            case UnknownTag:
                visibleTags << UnknownTag;
                break;
            default:
                break;
            }
        }
    }

    m_proxyModel->setVisibleTags(visibleTags);
}

void Structure::setDocument(ScxmlDocument *document)
{
    m_currentDocument = document;
    m_model->setDocument(document);
    m_proxyModel->invalidate();
    m_structureView->expandAll();
}

void Structure::setGraphicsScene(GraphicsScene *scene)
{
    m_scene = scene;
    connect(m_structureView, &TreeView::mouseExited, m_scene, &GraphicsScene::unhighlightAll);
}

void Structure::rowEntered(const QModelIndex &index)
{
    QTC_ASSERT(m_scene, return);

    QModelIndex ind = m_proxyModel->mapToSource(index);
    auto tag = static_cast<ScxmlTag*>(ind.internalPointer());
    if (tag)
        m_scene->highlightItems({tag});
    else
        m_scene->unhighlightAll();
}

void Structure::rowActivated(const QModelIndex &index)
{
    if (m_scene)
        m_scene->unselectAll();

    if (m_currentDocument) {
        QModelIndex ind = m_proxyModel->mapToSource(index);
        auto tag = static_cast<ScxmlTag*>(ind.internalPointer());
        if (tag)
            m_currentDocument->setCurrentTag(tag);
    }
}

void Structure::currentTagChanged(const QModelIndex &sourceIndex)
{
    QModelIndex ind = m_proxyModel->mapFromSource(sourceIndex);
    if (ind.isValid())
        m_structureView->setCurrentIndex(ind);
}

void Structure::childAdded(const QModelIndex &childIndex)
{
    m_proxyModel->invalidate();
    QModelIndex ind = m_proxyModel->mapFromSource(childIndex);
    if (ind.isValid()) {
        m_structureView->setCurrentIndex(ind);
        m_structureView->expand(ind.parent());
    }
}

void Structure::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace) {
        QModelIndex ind = m_proxyModel->mapToSource(m_structureView->currentIndex());
        auto tag = static_cast<ScxmlTag*>(ind.internalPointer());
        if (tag && m_currentDocument) {
            m_currentDocument->undoStack()->beginMacro(Tr::tr("Remove items"));
            m_currentDocument->removeTag(tag);
            m_currentDocument->undoStack()->endMacro();
        }
    }
    QFrame::keyPressEvent(e);
}

void Structure::createUi()
{
    auto titleLabel = new QLabel(Tr::tr("Structure"));
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_checkboxButton = new QToolButton;
    m_checkboxButton->setIcon(Utils::Icons::FILTER.icon());
    m_checkboxButton->setCheckable(true);

    auto toolBar = new QToolBar;
    toolBar->addWidget(titleLabel);
    toolBar->addWidget(m_checkboxButton);

    m_structureView = new TreeView;

    m_visibleTagsTitle = new QLabel;

    m_checkboxFrame = new QWidget;
    m_checkboxFrame->setLayout(new QVBoxLayout);
    m_checkboxFrame->layout()->setContentsMargins(0, 0, 0, 0);
    auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    m_tagVisibilityFrame = new QWidget;
    m_tagVisibilityFrame->setLayout(new QVBoxLayout);
    m_tagVisibilityFrame->layout()->addWidget(m_visibleTagsTitle);
    m_tagVisibilityFrame->layout()->addWidget(m_checkboxFrame);
    m_tagVisibilityFrame->layout()->addWidget(spacer);
    m_tagVisibilityFrame->layout()->setContentsMargins(0, 0, 0, 0);

    auto paneInnerFrame = new QWidget;
    paneInnerFrame->setLayout(new QHBoxLayout);
    paneInnerFrame->layout()->addWidget(m_structureView);
    paneInnerFrame->layout()->addWidget(m_tagVisibilityFrame);
    paneInnerFrame->layout()->setContentsMargins(0, 0, 0, 0);

    setLayout(new QVBoxLayout);
    layout()->addWidget(toolBar);
    layout()->addWidget(paneInnerFrame);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(0);
}

void Structure::showMenu(const QModelIndex &index, const QPoint &globalPos)
{
    if (index.isValid()) {
        QModelIndex ind = m_proxyModel->mapToSource(index);
        auto tag = static_cast<ScxmlTag*>(ind.internalPointer());
        if (tag) {
            auto menu = new QMenu;
            menu->addAction(Tr::tr("Expand All"), m_structureView, &TreeView::expandAll);
            menu->addAction(Tr::tr("Collapse All"), m_structureView, &TreeView::collapseAll);
            menu->addSeparator();
            menu->addAction(m_scene->actionHandler()->action(ActionCopy));
            menu->addAction(m_scene->actionHandler()->action(ActionPaste));
            menu->addSeparator();
            ScxmlUiFactory *uiFactory = m_scene->uiFactory();
            if (uiFactory) {
                auto actionProvider = static_cast<ActionProvider*>(uiFactory->object(Constants::C_UI_FACTORY_OBJECT_ACTIONPROVIDER));
                if (actionProvider) {
                    actionProvider->initStateMenu(tag, menu);
                    menu->addSeparator();
                }
            }

            TagUtils::createChildMenu(tag, menu);
            QAction *selectedAction = menu->exec(globalPos);
            if (selectedAction) {
                QVariantMap data = selectedAction->data().toMap();
                int actionType = data.value(Constants::C_SCXMLTAG_ACTIONTYPE, -1).toInt();
                if (actionType == TagUtils::Remove) {
                    m_currentDocument->undoStack()->beginMacro(Tr::tr("Remove items"));
                    m_currentDocument->setCurrentTag(tag);
                    m_currentDocument->removeTag(tag);
                    m_currentDocument->setCurrentTag(nullptr);
                    m_currentDocument->undoStack()->endMacro();
                } else if (actionType == TagUtils::AddChild) {
                    tag->document()->undoStack()->beginMacro(Tr::tr("Add child"));
                    ScxmlTag *childTag = (tag->tagType() == TagType::Else
                                          || tag->tagType() == TagType::ElseIf)
                            ? SceneUtils::addSibling(tag, data, m_scene)
                            : SceneUtils::addChild(tag, data, m_scene);
                    if (childTag && childTag->tagType() <= MetadataItem)
                        m_structureView->edit(m_structureView->currentIndex());
                    tag->document()->undoStack()->endMacro();
                }
            }

            m_proxyModel->invalidate();
            menu->deleteLater();
        }
    }
}
