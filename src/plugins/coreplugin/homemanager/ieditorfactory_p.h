#pragma once

#include <utils/mimetypes/mimetype.h>
#include <utils/mimetypes/mimedatabase.h>

#include <QHash>
#include <QSet>

namespace Core {

class IEditorFactory;

namespace Internal {

QHash<Utils::MimeType, IEditorFactory *> userPreferredEditorFactories();
void setUserPreferredEditorFactories(const QHash<Utils::MimeType, IEditorFactory *> &factories);

/* For something that has a 'QStringList mimeTypes' (IEditorFactory
 * or IExternalEditor), find the one best matching the mimetype passed in.
 *  Recurse over the parent classes of the mimetype to find them. */
template <class EditorFactoryLike>
static void mimeTypeFactoryLookup(const Utils::MimeType &mimeType,
                                     const QList<EditorFactoryLike*> &allFactories,
                                     QList<EditorFactoryLike*> *list)
{
    QSet<EditorFactoryLike *> matches;
    // search breadth-first through parent hierarchy, e.g. for hierarchy
    // * application/x-ruby
    //     * application/x-executable
    //         * application/octet-stream
    //     * text/plain
    QList<Utils::MimeType> queue;
    QSet<QString> seen;
    queue.append(mimeType);
    seen.insert(mimeType.name());
    while (!queue.isEmpty()) {
        Utils::MimeType mt = queue.takeFirst();
        // check for matching factories
        foreach (EditorFactoryLike *factory, allFactories) {
            if (!matches.contains(factory)) {
                foreach (const QString &mimeName, factory->mimeTypes()) {
                    if (mt.matchesName(mimeName)) {
                        list->append(factory);
                        matches.insert(factory);
                    }
                }
            }
        }
        // add parent mime types
        QStringList parentNames = mt.parentMimeTypes();
        foreach (const QString &parentName, parentNames) {
            const Utils::MimeType parent = Utils::mimeTypeForName(parentName);
            if (parent.isValid()) {
                int seenSize = seen.size();
                seen.insert(parent.name());
                if (seen.size() != seenSize) // not seen before, so add
                    queue.append(parent);
            }
        }
    }
}

} // Internal
} // Core
