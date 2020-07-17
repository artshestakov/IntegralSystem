#include "ISVersion.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISVersion::ISVersion()
{
	QSettings Settings(QFile::exists(PATH_BUILD_INFO) ? PATH_BUILD_INFO : ":Build/Build.ini", QSettings::IniFormat);
	Info.Major = Settings.value("Version/Major").toInt();
	Info.Minor = Settings.value("Version/Minor").toInt();
	Info.Revision = Settings.value("Version/Revision").toInt();
	Info.Date = Settings.value("Build/Date").toString().toStdString().c_str();
	Info.Time = Settings.value("Build/Time").toString().toStdString().c_str();
	Info.Hash = Settings.value("Build/Hash").toString().toStdString().c_str();
	Info.Branch = Settings.value("Build/Branch").toString().toStdString().c_str();
	Info.Configuration = Settings.value("Build/Configuration").toString().toStdString().c_str();
	Info.Platform = Settings.value("Build/Platform").toString().toStdString().c_str();
}
//-----------------------------------------------------------------------------
ISVersion::~ISVersion()
{

}
//-----------------------------------------------------------------------------
ISVersion& ISVersion::Instance()
{
	static ISVersion Version;
	return Version;
}
//-----------------------------------------------------------------------------
QString ISVersion::ToString() const
{
	return QString("%1.%2.%3").arg(Info.Major).arg(Info.Minor).arg(Info.Revision);
}
//-----------------------------------------------------------------------------
