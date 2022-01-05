#pragma once

#include "iexternalhome.h"

namespace Core {
namespace Internal {

class SystemEditor : public IExternalHome
{
    Q_OBJECT

public:
    explicit SystemEditor(QObject *parent = nullptr);

    QStringList mimeTypes() const override;
    Utils::Id id() const override;
    QString displayName() const override;

    bool startEditor(const Utils::FilePath &filePath, QString *errorMessage) override;
};

} // namespace Internal
} // namespace Core
