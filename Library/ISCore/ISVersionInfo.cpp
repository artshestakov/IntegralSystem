#include "ISVersionInfo.h"
#include "ISConstants.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISVersionInfo::ISVersionInfo()
{
	//Читаем информацию о версии
	QSettings Settings(QFile::exists(PATH_BUILD_INFO) ? PATH_BUILD_INFO : ":Build/Build.ini", QSettings::IniFormat);
	Info.Version = ISVersion(Settings.value("Version/Major").toUInt(), Settings.value("Version/Minor").toUInt(), Settings.value("Version/Revision").toUInt());
	Info.Date = Settings.value("Build/Date").toString();
	Info.Time = Settings.value("Build/Time").toString();
	Info.Hash = Settings.value("Build/Hash").toString();
	Info.Branch = Settings.value("Build/Branch").toString();
	Info.Configuration = Settings.value("Build/Configuration").toString();
	Info.Platform = Settings.value("Build/Platform").toString();

	//Описание всех конфигураций
	Configurations = std::map<QString, ISConfigurationInfo>
	{
		{ "Empty", { "{20DA4E7C-5843-49E1-9D87-2FF279EEF0FF}", QString(), LANG("Configuration.Empty"), QString(), QDate(), QString() } },
		{ "OilSphere", { "{B2003F73-0DD2-4359-AC96-FA7CABB53049}", QString(), LANG("Configuration.OilSphere"), QString(), QDate(), "Logo.png" } }
	};
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
QString ISVersionInfo::ToStringVersion() const
{
	return Info.Version.ToString();
}
//-----------------------------------------------------------------------------
void ISVersionInfo::SelectConfiguration(const QString &ConfigurationName)
{
	std::map<QString, ISConfigurationInfo>::const_iterator It = Configurations.find(ConfigurationName);
	IS_ASSERT(It != Configurations.end(), "Not found configuration \"" + ConfigurationName + "\"");
	ConfigurationInfo = It->second;
	ConfigurationInfo.Name = ConfigurationName;
}
//-----------------------------------------------------------------------------
