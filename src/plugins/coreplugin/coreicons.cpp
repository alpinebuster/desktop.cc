#include "coreicons.h"

using namespace Utils;

namespace Core {
namespace Icons {

const Icon QTCREATORLOGO_BIG(
        QLatin1String(":/core/images/logo/128/milab_logo.png"));
const Icon FIND_CASE_INSENSITIVELY(
        QLatin1String(":/find/images/casesensitively.png"));
const Icon FIND_WHOLE_WORD(
        QLatin1String(":/find/images/wholewords.png"));
const Icon FIND_REGEXP(
        QLatin1String(":/find/images/regexp.png"));
const Icon FIND_PRESERVE_CASE(
        QLatin1String(":/find/images/preservecase.png"));

const Icon MODE_HOME_CLASSIC(
        QLatin1String(":/fancyactionbar/images/mode_home@24x24.png"));
const Icon MODE_HOME_FLAT({
        {QLatin1String(":/fancyactionbar/images/mode_home_mask@24x24.png"),   Theme::IconsBaseColor}});
const Icon MODE_HOME_FLAT_ACTIVE({
        {QLatin1String(":/fancyactionbar/images/mode_home_mask@24x24.png"),   Theme::IconsModeEditActiveColor}});
const Icon MODE_DESIGN_CLASSIC(
        QLatin1String(":/fancyactionbar/images/mode_Design.png"));
const Icon MODE_DESIGN_FLAT({
        {QLatin1String(":/fancyactionbar/images/mode_design_mask.png"), Theme::IconsBaseColor}});
const Icon MODE_DESIGN_FLAT_ACTIVE({
        {QLatin1String(":/fancyactionbar/images/mode_design_mask.png"), Theme::IconsModeDesignActiveColor}});

} // namespace Icons
} // namespace Core
