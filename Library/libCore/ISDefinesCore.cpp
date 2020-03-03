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
QString ISDefines::Core::PATH_CONFIG_FILE;
QString ISDefines::Core::SYMBOL_CIRCLE;
QString ISDefines::Core::SYMBOL_MULTIPLY;
QString ISDefines::Core::SYMBOL_DIVIDE;
QString ISDefines::Core::SYMBOL_OBJECT_CHANGED;
QString ISDefines::Core::SYMBOL_SPACE_HIDE;
QString ISDefines::Core::SYMBOL_FIAS_SPLIT;
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
	PATH_CONFIG_FILE = PATH_APPLICATION_DIR + '/' + (ISDefines::Core::IS_GUI ? "Client" : "Server") + SYMBOL_POINT + EXTENSION_INI;
	SYMBOL_CIRCLE = QString(QChar(9679));
	SYMBOL_MULTIPLY = QString(QChar(215));
	SYMBOL_DIVIDE = QString(QChar(247));
	SYMBOL_OBJECT_CHANGED = QString(" {*}");
	SYMBOL_SPACE_HIDE = QString::fromLocal8Bit(" ");
	SYMBOL_FIAS_SPLIT = QString(SYMBOL_COMMA + SYMBOL_SPACE_HIDE);
}
//-----------------------------------------------------------------------------
