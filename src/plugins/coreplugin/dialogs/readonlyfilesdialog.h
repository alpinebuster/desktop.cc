#pragma once

#include <coreplugin/core_global.h>

#include <utils/fileutils.h>

#include <QDialog>

namespace Core {

class IDocument;

namespace Internal { class ReadOnlyFilesDialogPrivate; }

class CORE_EXPORT ReadOnlyFilesDialog : public QDialog
{
    Q_OBJECT

private:
    enum ReadOnlyFilesTreeColumn {
        MakeWritable = 0,
//        OpenWithVCS = 1,
        SaveAs = 1,
        FileName = 2,
        Folder = 3,
        NumberOfColumns
    };

public:
    enum ReadOnlyResult {
        RO_Cancel = -1,
//        RO_OpenVCS = OpenWithVCS,
        RO_MakeWritable = MakeWritable,
        RO_SaveAs = SaveAs
    };

    explicit ReadOnlyFilesDialog(const Utils::FilePaths &filePaths,
                                 QWidget *parent = nullptr);
    explicit ReadOnlyFilesDialog(const Utils::FilePath &filePath,
                                 QWidget * parent = nullptr);
    explicit ReadOnlyFilesDialog(IDocument *document,
                                 QWidget * parent = nullptr,
                                 bool displaySaveAs = false);
    explicit ReadOnlyFilesDialog(const QList<IDocument *> &documents,
                                 QWidget * parent = nullptr);

    ~ReadOnlyFilesDialog() override;

    void setMessage(const QString &message);
    void setShowFailWarning(bool show, const QString &warning = QString());

    int exec() override;

private:
    friend class Internal::ReadOnlyFilesDialogPrivate;
    Internal::ReadOnlyFilesDialogPrivate *d;
};

} // namespace Core
