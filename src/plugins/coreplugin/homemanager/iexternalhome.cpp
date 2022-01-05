#include "iexternalhome.h"

#include "ieditorfactory_p.h"

namespace Core {

/*!
    \class Core::IExternalEditor
    \inheaderfile coreplugin/editormanager/iexternaleditor.h
    \inmodule QtCreator
    \ingroup mainclasses

    \brief The IExternalEditor class enables registering an external
    editor in the \uicontrol{Open With} dialog.
*/

/*!
    \fn QString Core::IExternalEditor::displayName() const
    Returns a user-visible description of the editor type.
*/

/*!
    \fn Utils::Id Core::IExternalEditor::id() const
    Returns the ID of the factory or editor type.
*/

/*!
    \fn QStringList Core::IExternalEditor::mimeTypes() const
    Returns a list of MIME types that the editor supports
*/

/*!

    \fn bool Core::IExternalEditor::startEditor(const QString &fileName, QString *errorMessage) = 0;

    Opens the editor with \a fileName. Returns \c true on success or \c false
    on failure along with the error in \a errorMessage.
*/

static QList<IExternalHome *> g_externalEditors;

/*!
    \internal
*/
IExternalHome::IExternalHome(QObject *parent)
    : QObject(parent)
{
    g_externalEditors.append(this);
}

/*!
    \internal
*/
IExternalHome::~IExternalHome()
{
    g_externalEditors.removeOne(this);
}

/*!
    Returns all available external editors.
*/
const ExternalEditorList IExternalHome::allExternalEditors()
{
    return g_externalEditors;
}

/*!
    Returns all external editors available for this \a mimeType in the default
    order (editors ordered by MIME type hierarchy).
*/
const ExternalEditorList IExternalHome::externalEditors(const Utils::MimeType &mimeType)
{
    ExternalEditorList rc;
    const ExternalEditorList allEditors = IExternalHome::allExternalEditors();
    Internal::mimeTypeFactoryLookup(mimeType, allEditors, &rc);
    return rc;
}

} // Core
