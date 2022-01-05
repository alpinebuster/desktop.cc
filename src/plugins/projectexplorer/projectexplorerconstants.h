#pragma once

#include "projectexplorer_export.h"

namespace ProjectExplorer {
namespace Constants {

// Navigation widget
const char FOLDER_NAVIGATION_WIDGET_FACTORY_ID[] = "Working";

// Modes and their priorities
const char MODE_SESSION[]         = "Project";

// Actions
const char BUILD[]                = "ProjectExplorer.Build";
const char STOP[]                 = "ProjectExplorer.Stop";
const char ADDNEWFILE[]           = "ProjectExplorer.AddNewFile";
const char FILEPROPERTIES[]       = "ProjectExplorer.FileProperties";
const char RENAMEFILE[]           = "ProjectExplorer.RenameFile";
const char REMOVEFILE[]           = "ProjectExplorer.RemoveFile";

// Context
const char C_PROJECTEXPLORER[]    = "Project Explorer";
const char C_PROJECT_TREE[]       = "ProjectExplorer.ProjectTreeContext";

// Menus
const char M_BUILDPROJECT[]       = "ProjectExplorer.Menu.Build";
const char M_DEBUG[]              = "ProjectExplorer.Menu.Debug";
const char M_DEBUG_STARTDEBUGGING[] = "ProjectExplorer.Menu.Debug.StartDebugging";

// Menu groups
const char G_BUILD_BUILD[]        = "ProjectExplorer.Group.Build";
const char G_BUILD_ALLPROJECTS[]  = "ProjectExplorer.Group.AllProjects";
const char G_BUILD_PROJECT[]      = "ProjectExplorer.Group.Project";
const char G_BUILD_PRODUCT[]      = "ProjectExplorer.Group.Product";
const char G_BUILD_SUBPROJECT[]   = "ProjectExplorer.Group.SubProject";
const char G_BUILD_FILE[]         = "ProjectExplorer.Group.File";
const char G_BUILD_ALLPROJECTS_ALLCONFIGURATIONS[] = "ProjectExplorer.Group.AllProjects.AllConfigurations";
const char G_BUILD_PROJECT_ALLCONFIGURATIONS[] = "ProjectExplorer.Group.Project.AllConfigurations";
const char G_BUILD_RUN[]          = "ProjectExplorer.Group.Run";
const char G_BUILD_CANCEL[]       = "ProjectExplorer.Group.BuildCancel";

// Context menus
const char M_SESSIONCONTEXT[]     = "Project.Menu.Session";
const char M_PROJECTCONTEXT[]     = "Project.Menu.Project";
const char M_SUBPROJECTCONTEXT[]  = "Project.Menu.SubProject";
const char M_FOLDERCONTEXT[]      = "Project.Menu.Folder";
const char M_FILECONTEXT[]        = "Project.Menu.File";
const char M_OPENFILEWITHCONTEXT[] = "Project.Menu.File.OpenWith";
const char M_OPENTERMINALCONTEXT[] = "Project.Menu.File.OpenTerminal";

// Context menu groups
const char G_SESSION_BUILD[]      = "Session.Group.Build";
const char G_SESSION_REBUILD[]     = "Session.Group.Rebuild";
const char G_SESSION_FILES[]      = "Session.Group.Files";
const char G_SESSION_OTHER[]      = "Session.Group.Other";

const char G_PROJECT_FIRST[]      = "Project.Group.Open";
const char G_PROJECT_BUILD[]      = "Project.Group.Build";
const char G_PROJECT_REBUILD[]    = "Project.Group.Rebuild";
const char G_PROJECT_RUN[]        = "Project.Group.Run";
const char G_PROJECT_FILES[]      = "Project.Group.Files";
const char G_PROJECT_TREE[]       = "Project.Group.Tree";
const char G_PROJECT_LAST[]       = "Project.Group.Last";

const char G_FOLDER_LOCATIONS[]   = "ProjectFolder.Group.Locations";
const char G_FOLDER_FILES[]       = "ProjectFolder.Group.Files";
const char G_FOLDER_OTHER[]       = "ProjectFolder.Group.Other";
const char G_FOLDER_CONFIG[]      = "ProjectFolder.Group.Config";

const char G_FILE_OPEN[]          = "ProjectFile.Group.Open";
const char G_FILE_OTHER[]         = "ProjectFile.Group.Other";
const char G_FILE_CONFIG[]        = "ProjectFile.Group.Config";

// Mime types
const char C_SOURCE_MIMETYPE[]    = "text/x-csrc";
const char C_HEADER_MIMETYPE[]    = "text/x-chdr";
const char CPP_SOURCE_MIMETYPE[]  = "text/x-c++src";
const char CPP_HEADER_MIMETYPE[]  = "text/x-c++hdr";
const char LINGUIST_MIMETYPE[]    = "text/vnd.trolltech.linguist";
const char FORM_MIMETYPE[]        = "application/x-designer";
const char QML_MIMETYPE[]         = "text/x-qml"; // separate def also in qmljstoolsconstants.h
const char QMLUI_MIMETYPE[]       = "application/x-qt.ui+qml";
const char RESOURCE_MIMETYPE[]    = "application/vnd.qt.xml.resource";
const char SCXML_MIMETYPE[]       = "application/scxml+xml";

// Default directory to run custom (build) commands in.
const char DEFAULT_WORKING_DIR[] = "%{buildDir}";
const char DEFAULT_WORKING_DIR_ALTERNATE[] = "%{sourceDir}";

// JsonWizard:
const char PAGE_ID_PREFIX[] = "PE.Wizard.Page.";
const char GENERATOR_ID_PREFIX[] = "PE.Wizard.Generator.";

// File icon overlays
const char FILEOVERLAY_QT[]=":/projectexplorer/images/fileoverlay_qt.png";
const char FILEOVERLAY_GROUP[] = ":/projectexplorer/images/fileoverlay_group.png";
const char FILEOVERLAY_PRODUCT[] = ":/projectexplorer/images/fileoverlay_product.png";
const char FILEOVERLAY_MODULES[] = ":/projectexplorer/images/fileoverlay_modules.png";
const char FILEOVERLAY_QML[]=":/projectexplorer/images/fileoverlay_qml.png";
const char FILEOVERLAY_UI[]=":/projectexplorer/images/fileoverlay_ui.png";
const char FILEOVERLAY_QRC[]=":/projectexplorer/images/fileoverlay_qrc.png";
const char FILEOVERLAY_CPP[]=":/projectexplorer/images/fileoverlay_cpp.png";
const char FILEOVERLAY_H[]=":/projectexplorer/images/fileoverlay_h.png";
const char FILEOVERLAY_SCXML[]=":/projectexplorer/images/fileoverlay_scxml.png";
const char FILEOVERLAY_PY[]=":/projectexplorer/images/fileoverlay_py.png";
const char FILEOVERLAY_UNKNOWN[]=":/projectexplorer/images/fileoverlay_unknown.png";

// Settings
const char ADD_FILES_DIALOG_FILTER_HISTORY_KEY[] = "ProjectExplorer.AddFilesFilterKey";
const char PROJECT_ROOT_PATH_KEY[] = "ProjectExplorer.Project.RootPath";
const char STARTUPSESSION_KEY[] = "ProjectExplorer/SessionToRestore";
const char LASTSESSION_KEY[] = "ProjectExplorer/StartupSession";

// UI texts
PROJECTEXPLORER_EXPORT QString msgAutoDetected();
PROJECTEXPLORER_EXPORT QString msgAutoDetectedToolTip();
PROJECTEXPLORER_EXPORT QString msgManual();

} // namespace Constants
} // namespace ProjectExplorer
