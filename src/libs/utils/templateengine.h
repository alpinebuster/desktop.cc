#pragma once

#include "utils_global.h"

#include "macroexpander.h"

#include <QString>

QT_FORWARD_DECLARE_CLASS(QJSEngine);

namespace Utils {

class QTCREATOR_UTILS_EXPORT TemplateEngine {
public:
    static bool preprocessText(const QString &input, QString *output, QString *errorMessage);

    static QString processText(MacroExpander *expander, const QString &input,
                               QString *errorMessage);

    static bool evaluateBooleanJavaScriptExpression(QJSEngine &engine, const QString &expression,
                                                    bool *result, QString *errorMessage);
};

} // namespace Utils
