#include "ISConfigurations.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISConfigurations::ISConfigurations()
{
	//������ ������������
	ISConfigurationInfo Empty;
	Empty.Name = "Empty";
	Empty.UID = "{20DA4E7C-5843-49E1-9D87-2FF279EEF0FF}";
	Empty.LocalName = LANG("Configuration.Empty");
	Empty.DesktopForm = QString();
	Empty.DateExpired = QDate();
	Empty.LogoName = QString();
	Vector.emplace_back(Empty);

	//����������
	ISConfigurationInfo OilSphere;
	OilSphere.Name = "OilSphere";
	OilSphere.UID = "{B2003F73-0DD2-4359-AC96-FA7CABB53049}";
	OilSphere.LocalName = LANG("Configuration.OilSphere");
	OilSphere.DesktopForm = QString();
	OilSphere.DateExpired = QDate();
	OilSphere.LogoName = "Logo.png";
	Vector.emplace_back(OilSphere);
}
//-----------------------------------------------------------------------------
ISConfigurations::~ISConfigurations()
{

}
//-----------------------------------------------------------------------------
ISConfigurations& ISConfigurations::Instance()
{
	static ISConfigurations Configurations;
	return Configurations;
}
//-----------------------------------------------------------------------------
ISConfigurationInfo& ISConfigurations::Get()
{
	return Current;
}
//-----------------------------------------------------------------------------
bool ISConfigurations::Set(const QString &configuration_name)
{
	bool Result = false;
	for (const ISConfigurationInfo &ConfigurationInfo : Vector)
	{
		if (ConfigurationInfo.Name == configuration_name)
		{
			Current = ConfigurationInfo;
			Result = true;
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------