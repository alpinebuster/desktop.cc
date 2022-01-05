#pragma once

namespace Utils {

enum OutputFormat
{
    NormalMessageFormat,
    ErrorMessageFormat,
    LogMessageFormat,
    DebugFormat,
    StdOutFormat,
    StdErrFormat,
    GeneralMessageFormat,
    NumberOfFormats // Keep this entry last.
};

} // namespace Utils
