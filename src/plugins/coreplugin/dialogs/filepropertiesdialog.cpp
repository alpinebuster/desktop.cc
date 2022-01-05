#include "filepropertiesdialog.h"
#include "ui_filepropertiesdialog.h"

#include <coreplugin/homemanager/homemanager.h>
#include <coreplugin/homemanager/ieditorfactory.h>
#include <utils/fileutils.h>
#include <utils/mimetypes/mimedatabase.h>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLocale>

FilePropertiesDialog::FilePropertiesDialog(const Utils::FilePath &fileName, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::FilePropertiesDialog),
    m_fileName(fileName.toString())
{
    m_ui->setupUi(this);

    connect(m_ui->readable, &QCheckBox::clicked, [this](bool checked) {
        setPermission(QFile::ReadUser | QFile::ReadOwner, checked);
    });
    connect(m_ui->writable, &QCheckBox::clicked, [this](bool checked) {
        setPermission(QFile::WriteUser | QFile::WriteOwner, checked);
    });
    connect(m_ui->executable, &QCheckBox::clicked, [this](bool checked) {
        setPermission(QFile::ExeUser | QFile::ExeOwner, checked);
    });

    refresh();
}

FilePropertiesDialog::~FilePropertiesDialog()
{
    delete m_ui;
}

void FilePropertiesDialog::detectTextFileSettings()
{
    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        m_ui->lineEndings->setText(tr("Unknown"));
        m_ui->indentation->setText(tr("Unknown"));
        return;
    }

    char lineSeparator = '\n';
    const QByteArray data = file.read(50000);
    file.close();

    // Try to guess the files line endings
    if (data.contains("\r\n")) {
        m_ui->lineEndings->setText(tr("Windows (CRLF)"));
    } else if (data.contains("\n")) {
        m_ui->lineEndings->setText(tr("Unix (LF)"));
    } else if (data.contains("\r")) {
        m_ui->lineEndings->setText(tr("Mac (CR)"));
        lineSeparator = '\r';
    } else {
        // That does not look like a text file at all
        m_ui->lineEndings->setText(tr("Unknown"));
        return;
    }

    auto leadingSpaces = [](const QByteArray &line) {
        for (int i = 0, max = line.size(); i < max; ++i) {
            if (line.at(i) != ' ') {
                return i;
            }
        }
        return 0;
    };

    // Try to guess the files indentation style
    bool tabIndented = false;
    int lastLineIndent = 0;
    std::map<int, int> indents;
    const QList<QByteArray> list = data.split(lineSeparator);
    for (const QByteArray &line : list) {
        if (line.startsWith(' ')) {
            int spaces = leadingSpaces(line);
            int relativeCurrentLineIndent = qAbs(spaces - lastLineIndent);
            // Ignore zero or one character indentation changes
            if (relativeCurrentLineIndent < 2)
                continue;
            indents[relativeCurrentLineIndent]++;
            lastLineIndent = spaces;
        } else if (line.startsWith('\t')) {
            tabIndented = true;
        }

        if (!indents.empty() && tabIndented)
            break;
    }

    const std::map<int, int>::iterator max = std::max_element(
                indents.begin(), indents.end(),
                [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
        return a.second < b.second;
    });

    if (!indents.empty()) {
        if (tabIndented) {
            m_ui->indentation->setText(tr("Mixed"));
        } else {
            m_ui->indentation->setText(tr("%1 Spaces").arg(max->first));
        }
    } else if (tabIndented) {
        m_ui->indentation->setText(tr("Tabs"));
    } else {
        m_ui->indentation->setText(tr("Unknown"));
    }
}

void FilePropertiesDialog::refresh()
{
    Utils::withNtfsPermissions<void>([this] {
        const QFileInfo fileInfo(m_fileName);
        QLocale locale;

        m_ui->name->setText(fileInfo.fileName());
        m_ui->path->setText(QDir::toNativeSeparators(fileInfo.canonicalPath()));

        const Utils::MimeType mimeType = Utils::mimeTypeForFile(fileInfo);
        m_ui->mimeType->setText(mimeType.name());

        const Core::HomeFactoryList factories = Core::IEditorFactory::preferredEditorFactories(m_fileName);
        m_ui->defaultEditor->setText(!factories.isEmpty() ? factories.at(0)->displayName() : tr("Undefined"));

        m_ui->owner->setText(fileInfo.owner());
        m_ui->group->setText(fileInfo.group());
        m_ui->size->setText(locale.formattedDataSize(fileInfo.size()));
        m_ui->readable->setChecked(fileInfo.isReadable());
        m_ui->writable->setChecked(fileInfo.isWritable());
        m_ui->executable->setChecked(fileInfo.isExecutable());
        m_ui->symLink->setChecked(fileInfo.isSymLink());
        m_ui->lastRead->setText(fileInfo.lastRead().toString(locale.dateTimeFormat()));
        m_ui->lastModified->setText(fileInfo.lastModified().toString(locale.dateTimeFormat()));
        if (mimeType.inherits("text/plain")) {
            detectTextFileSettings();
        } else {
            m_ui->lineEndings->setText(tr("Unknown"));
            m_ui->indentation->setText(tr("Unknown"));
        }
    });
}

void FilePropertiesDialog::setPermission(QFile::Permissions newPermissions, bool set)
{
    Utils::withNtfsPermissions<void>([this, newPermissions, set] {
        QFile::Permissions permissions = QFile::permissions(m_fileName);
        if (set)
            permissions |= newPermissions;
        else
            permissions &= ~newPermissions;

        if (!QFile::setPermissions(m_fileName, permissions))
            qWarning() << "Cannot change permissions for file" << m_fileName;
    });

    refresh();
}
