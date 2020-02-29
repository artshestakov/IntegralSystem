#include "ISDefinesCore.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDefinesCore::ISDefinesCore()
{

}
//-----------------------------------------------------------------------------
ISDefinesCore::~ISDefinesCore()
{

}
//-----------------------------------------------------------------------------
ISDefinesCore& ISDefinesCore::Instance()
{
	static ISDefinesCore Defines;
	return Defines;
}
//-----------------------------------------------------------------------------
void ISDefinesCore::Init(bool IsGui)
{
	IS_GUI = IsGui;
	APPLICATION_NAME = QCoreApplication::applicationName();
	PATH_APPLICATION_DIR = QCoreApplication::applicationDirPath();
	PATH_APPLICATION_FILE = QCoreApplication::applicationFilePath();
	PATH_LOGS_DIR = PATH_APPLICATION_DIR + "/Logs";
	PATH_TEMP_DIR = PATH_APPLICATION_DIR + "/Temp";
	PATH_CRASH_DIR = PATH_APPLICATION_DIR + "/Crash";
	PATH_TRANSLATIONS_DIR = PATH_APPLICATION_DIR + "/translations";
	PATH_CONFIG_FILE = PATH_APPLICATION_DIR + '/' + APPLICATION_NAME + SYMBOL_POINT + EXTENSION_INI;
	SYMBOL_CIRCLE = QString(QChar(9679));
	SYMBOL_MULTIPLY = QString(QChar(215));
	SYMBOL_DIVIDE = QString(QChar(247));
	SYMBOL_OBJECT_CHANGED = QString(" {*}");
	SYMBOL_SPACE_HIDE = QString::fromLocal8Bit(" ");
	SYMBOL_FIAS_SPLIT = QString(SYMBOL_COMMA + SYMBOL_SPACE_HIDE);
}
//-----------------------------------------------------------------------------
