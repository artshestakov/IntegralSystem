#include "ISVersionInfo.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISVersionInfo::ISVersionInfo()
{
	QSettings Settings(QFile::exists(PATH_BUILD_INFO) ? PATH_BUILD_INFO : ":Build/Build.ini", QSettings::IniFormat);
	Info.Version = ISVersion(Settings.value("Version/Major").toUInt(), Settings.value("Version/Minor").toUInt(), Settings.value("Version/Revision").toUInt());
	Info.Date = Settings.value("Build/Date").toString();
	Info.Time = Settings.value("Build/Time").toString();
	Info.Hash = Settings.value("Build/Hash").toString();
	Info.Branch = Settings.value("Build/Branch").toString();
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
QString ISVersionInfo::ToString() const
{
	return Info.Version.ToString();
}
//-----------------------------------------------------------------------------
