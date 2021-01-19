#include "ISVersionInfo.h"
#include "ISConstants.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISVersionInfo::ISVersionInfo()
{
	//Читаем информацию о версии
	QSettings Settings(QFile::exists(PATH_BUILD_INFO) ? PATH_BUILD_INFO : ":Build/Build.ini", QSettings::IniFormat);
	Info.Version = Settings.value("Version/Number").toUInt();
	Info.Date = Settings.value("Build/Date").toString();
	Info.Time = Settings.value("Build/Time").toString();
	Info.Hash = Settings.value("Build/Hash").toString();
	Info.BranchName = Settings.value("Build/BranchName").toString();
	Info.Configuration = Settings.value("Build/Configuration").toString();
	Info.Platform = Settings.value("Build/Platform").toString();
}
//-----------------------------------------------------------------------------
ISVersionInfo::~ISVersionInfo()
{

}
//-----------------------------------------------------------------------------
ISVersionInfo& ISVersionInfo::Instance()
{
	static ISVersionInfo Version;
	return Version;
}
//-----------------------------------------------------------------------------
