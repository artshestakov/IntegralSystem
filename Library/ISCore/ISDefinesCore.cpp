#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
bool ISDefines::Core::IS_GUI;
QString ISDefines::Core::APPLICATION_NAME;
QString ISDefines::Core::PATH_APPLICATION_DIR;
QString ISDefines::Core::PATH_APPLICATION_FILE;
QString ISDefines::Core::PATH_LOGS_DIR;
QString ISDefines::Core::PATH_TEMP_DIR;
QString ISDefines::Core::PATH_CRASH_DIR;
QString ISDefines::Core::PATH_TRANSLATIONS_DIR;
QString ISDefines::Core::PATH_PID_FILE;
QString ISDefines::Core::PATH_LAST_DIRECTORY;
QString ISDefines::Core::SYMBOL_CIRCLE;
QString ISDefines::Core::SYMBOL_OBJECT_CHANGED;
//-----------------------------------------------------------------------------
void ISDefines::Core::Init(bool IsGui)
{
	IS_GUI = IsGui;
	APPLICATION_NAME = QCoreApplication::applicationName();
	PATH_APPLICATION_DIR = QCoreApplication::applicationDirPath();
	PATH_APPLICATION_FILE = QCoreApplication::applicationFilePath();
	PATH_LOGS_DIR = PATH_APPLICATION_DIR + "/Logs";
	PATH_TEMP_DIR = PATH_APPLICATION_DIR + "/Temp";
	PATH_CRASH_DIR = PATH_APPLICATION_DIR + "/Crash";
	PATH_TRANSLATIONS_DIR = PATH_APPLICATION_DIR + "/translations";
    PATH_PID_FILE = PATH_TEMP_DIR + PATH_SEPARATOR + APPLICATION_NAME + '.' + EXTENSION_PID;
	PATH_LAST_DIRECTORY = QDir::homePath();
	SYMBOL_CIRCLE = QString(QChar(9679));
	SYMBOL_OBJECT_CHANGED = QString(" {*}");
}
//-----------------------------------------------------------------------------
