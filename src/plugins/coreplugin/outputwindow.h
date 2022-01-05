#pragma once

#include "core_global.h"
#include "icontext.h"

#include <utils/outputformat.h>

#include <QPlainTextEdit>

namespace Utils {
class OutputFormatter;
class OutputLineParser;
}

namespace Core {

namespace Internal { class OutputWindowPrivate; }

class CORE_EXPORT OutputWindow : public QPlainTextEdit
{
    Q_OBJECT

public:
    enum class FilterModeFlag {
        Default       = 0x00, // Plain text, non case sensitive, for initialization
        RegExp        = 0x01,
        CaseSensitive = 0x02,
        Inverted      = 0x04,
    };
    Q_DECLARE_FLAGS(FilterModeFlags, FilterModeFlag)

    OutputWindow(Context context, const QString &settingsKey, QWidget *parent = nullptr);
    ~OutputWindow() override;

    void setLineParsers(const QList<Utils::OutputLineParser *> &parsers);
    Utils::OutputFormatter *outputFormatter() const;

    void appendMessage(const QString &out, Utils::OutputFormat format);

    void grayOutOldContent();
    void clear();
    void flush();
    void reset();

    void scrollToBottom();

    void setMaxCharCount(int count);
    int maxCharCount() const;

    void setBaseFont(const QFont &newFont);
    float fontZoom() const;
    void setFontZoom(float zoom);
    void resetZoom() { setFontZoom(0); }
    void setWheelZoomEnabled(bool enabled);

    void updateFilterProperties(
            const QString &filterText,
            Qt::CaseSensitivity caseSensitivity,
            bool regexp,
            bool isInverted);

signals:
    void wheelZoom();

public slots:
    void setWordWrapEnabled(bool wrap);

protected:
    virtual void handleLink(const QPoint &pos);

private:
    QMimeData *createMimeDataFromSelection() const override;
    void keyPressEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void showEvent(QShowEvent *) override;
    void wheelEvent(QWheelEvent *e) override;

    using QPlainTextEdit::setFont; // call setBaseFont instead, which respects the zoom factor
    void enableUndoRedo();
    void filterNewContent();
    void handleNextOutputChunk();
    void handleOutputChunk(const QString &output, Utils::OutputFormat format);
    void updateAutoScroll();

    Internal::OutputWindowPrivate *d = nullptr;
};

} // namespace Core
