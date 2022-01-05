#include "idocumentfactory.h"

#include <utils/qtcassert.h>

namespace Core {

static QList<IDocumentFactory *> g_documentFactories;

IDocumentFactory::IDocumentFactory()
{
    g_documentFactories.append(this);
}

IDocumentFactory::~IDocumentFactory()
{
    g_documentFactories.removeOne(this);
}

const QList<IDocumentFactory *> IDocumentFactory::allDocumentFactories()
{
    return g_documentFactories;
}

IDocument *IDocumentFactory::open(const QString &filename)
{
    QTC_ASSERT(m_opener, return nullptr);
    return m_opener(filename);
}

} // namespace Core
