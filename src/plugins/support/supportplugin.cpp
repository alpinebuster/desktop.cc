#include "supportplugin.h"

#include "gettingstartedwelcomepage.h"
#include "qtsupportconstants.h"
#include <coreplugin/icore.h>
#include <coreplugin/jsexpander.h>

#include <projectexplorer/projectexplorer.h>

#include <utils/infobar.h>
#include <utils/macroexpander.h>

using namespace Core;
using namespace ProjectExplorer;

namespace Support {
namespace Internal {

class SupportPluginPrivate
{
public:
    ExamplesWelcomePage examplesPage{true};
    ExamplesWelcomePage tutorialPage{false};
};

SupportPlugin::~SupportPlugin()
{
    delete d;
}

bool SupportPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    d = new SupportPluginPrivate;

    return true;
}

const char kLoginkWithMFDSSetting[] = "LoginWithMIFDS";

static void askAboutLogin()
{
    Utils::InfoBarEntry info(
        kLoginkWithMFDSSetting,
        SupportPlugin::tr(
            "Login with a MFDS Account to automatically sync your patients and settings? To do "
            "this later, select Welcome > Account > Login to MFDS."),
        Utils::InfoBarEntry::GlobalSuppression::Enabled);
    info.setCustomButtonInfo(SupportPlugin::tr("Login to MFDS"), [] {
        ICore::infoBar()->removeInfo(kLoginkWithMFDSSetting);
    });
    ICore::infoBar()->addInfo(info);
}

void SupportPlugin::extensionsInitialized()
{
    askAboutLogin();
}

} // Internal
} // QtSupport
