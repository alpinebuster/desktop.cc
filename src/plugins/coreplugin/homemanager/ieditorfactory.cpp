#include "ieditorfactory.h"
#include "ieditorfactory_p.h"
#include "homemanager.h"

#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>

#include <QFileInfo>

namespace Core {

/*!
    \class Core::IEditorFactory
    \inheaderfile coreplugin/editormanager/ieditorfactory.h
    \inmodule QtCreator

    \brief The IEditorFactory class creates suitable editors for documents
    according to their MIME type.

    Whenever a user wants to edit or create a document, the EditorManager
    scans all IEditorFactory instances for suitable editors. The selected
    IEditorFactory is then asked to create an editor.

    Implementations should set the properties of the IEditorFactory subclass in
    their constructor.

    IEditorFactory instances automatically register themselves in \QC in their
    constructor.

    \sa Core::IEditor
    \sa Core::IDocument
    \sa Core::EditorManager
*/

/*!
    \fn void Core::IEditorFactory::addMimeType(const QString &mimeType)

    Adds \a mimeType to the list of MIME types supported by this editor type.

    \sa mimeTypes()
    \sa setMimeTypes()
*/

/*!
    \fn QString Core::IEditorFactory::displayName() const

    Returns a user-visible description of the editor type.

    \sa setDisplayName()
*/

/*!
    \fn Utils::Id Core::IEditorFactory::id() const

    Returns the ID of the editors' document type.

    \sa setId()
*/

/*!
    \fn QString Core::IEditorFactory::mimeTypes() const

    Returns the list of supported MIME types of this editor factory.

    \sa addMimeType()
    \sa setMimeTypes()
*/

/*!
    \fn void Core::IEditorFactory::setDisplayName(const QString &displayName)

    Sets the \a displayName of the editor type. This is for example shown in
    the \uicontrol {Open With} menu and the MIME type preferences.

    \sa displayName()
*/

/*!
    \fn void Core::IEditorFactory::setId(Utils::Id id)

    Sets the \a id of the editors' document type. This must be the same as the
    IDocument::id() of the documents returned by created editors.

    \sa id()
*/

/*!
    \fn void Core::IEditorFactory::setMimeTypes(const QStringList &mimeTypes)

    Sets the MIME types supported by the editor type to \a mimeTypes.

    \sa addMimeType()
    \sa mimeTypes()
*/

static QList<IEditorFactory *> g_editorFactories;
static QHash<Utils::MimeType, IEditorFactory *> g_userPreferredEditorFactories;

/*!
    Creates an IEditorFactory.

    Registers the IEditorFactory in \QC.
*/
IEditorFactory::IEditorFactory()
{
    g_editorFactories.append(this);
}

/*!
    \internal
*/
IEditorFactory::~IEditorFactory()
{
    g_editorFactories.removeOne(this);
}

/*!
    \internal
*/
const HomeFactoryList IEditorFactory::allEditorFactories()
{
    return g_editorFactories;
}

/*!
    Returns all available editor factories for the \a mimeType in the default
    order (editor types ordered by MIME type hierarchy).
*/
const HomeFactoryList IEditorFactory::defaultEditorFactories(const Utils::MimeType &mimeType)
{
    HomeFactoryList rc;
    const HomeFactoryList allFactories = IEditorFactory::allEditorFactories();
    Internal::mimeTypeFactoryLookup(mimeType, allFactories, &rc);
    return rc;
}

/*!
    Returns the available editor factories for \a fileName in order of
    preference. That is the default order for the document's MIME type but with
    a user overridden default editor first, and the binary editor as the very
    first item if a text document is too large to be opened as a text file.
*/
const HomeFactoryList IEditorFactory::preferredEditorFactories(const QString &fileName)
{
    const QFileInfo fileInfo(fileName);
    // default factories by mime type
    const Utils::MimeType mimeType = Utils::mimeTypeForFile(fileInfo);
    HomeFactoryList factories = defaultEditorFactories(mimeType);
    const auto factories_moveToFront = [&factories](IEditorFactory *f) {
        factories.removeAll(f);
        factories.prepend(f);
    };
    // user preferred factory to front
    IEditorFactory *userPreferred = Internal::userPreferredEditorFactories().value(mimeType);
    if (userPreferred)
        factories_moveToFront(userPreferred);
    // open text files > 48 MB in binary editor
    if (fileInfo.size() > HomeManager::maxTextFileSize()
            && mimeType.inherits("text/plain")) {
        const Utils::MimeType binary = Utils::mimeTypeForName("application/octet-stream");
        const HomeFactoryList binaryEditors = defaultEditorFactories(binary);
        if (!binaryEditors.isEmpty())
            factories_moveToFront(binaryEditors.first());
    }
    return factories;
}

/*!
    Creates an editor.

    Uses the function set with setEditorCreator() to create the editor.

    \sa setEditorCreator()
*/
IEditor *IEditorFactory::createEditor() const
{
    QTC_ASSERT(m_creator, return nullptr);
    return m_creator();
}

/*!
    Sets the function that is used to create an editor instance in
    createEditor() to \a creator.

    \sa createEditor()
*/
void IEditorFactory::setEditorCreator(const std::function<IEditor *()> &creator)
{
    m_creator = creator;
}

/*!
    \internal
*/
QHash<Utils::MimeType, Core::IEditorFactory *> Core::Internal::userPreferredEditorFactories()
{
    return g_userPreferredEditorFactories;
}

/*!
    \internal
*/
void Internal::setUserPreferredEditorFactories(const QHash<Utils::MimeType, IEditorFactory *> &factories)
{
    g_userPreferredEditorFactories = factories;
}

} // Core
