#include "ISConfigurations.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISConfigurations::ISConfigurations()
    : Index(0)
{
	//Пустая конфигурация
	ISConfigurationInfo Empty;
    Empty.Type = ISNamespace::ConfigurationType::Empty;
	Empty.Name = "Empty";
	Empty.UID = "{20DA4E7C-5843-49E1-9D87-2FF279EEF0FF}";
    Empty.LocalName = LANG("Configuration.Empty");
	Empty.DesktopForm = std::string();
	Empty.DateExpired = ISDate();
	Empty.LogoName = std::string();
	Vector.emplace_back(Empty);

	//Нефтесфера
	ISConfigurationInfo OilSphere;
    OilSphere.Type = ISNamespace::ConfigurationType::OilSphere;
	OilSphere.Name = "OilSphere";
	OilSphere.UID = "{B2003F73-0DD2-4359-AC96-FA7CABB53049}";
    OilSphere.LocalName = LANG("Configuration.OilSphere");
	OilSphere.DesktopForm = std::string();
	OilSphere.DateExpired = ISDate();
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
const ISConfigurationInfo& ISConfigurations::Get()
{
	return Vector[Index];
}
//-----------------------------------------------------------------------------
bool ISConfigurations::Set(const std::string &configuration_name)
{
	bool Result = false;
	for (size_t i = 0, c = Vector.size(); i < c; ++i)
	{
		if (Vector[i].Name == configuration_name)
		{
			Index = i;
			Result = true;
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
