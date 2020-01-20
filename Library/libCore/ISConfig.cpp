#include "ISConfig.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISDebug.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: Settings(nullptr)
{
	Structure =
	{
		{
			"Connection",
			{
				{ "Server" , "" },
				{ "Port", "" },
				{ "Database", "" },
				{ "Login", "" },
				{ "Password", "" }
			}
		},
		{
			"AutoInput",
			{
				{ "Included" , "false" },
				{ "Login", "" },
				{ "Password", "" }
			}
		},
		{
			"Other",
			{
				{ "Autoboot" , "false" },
				{ "Language", "ru" }
			}
		},
		{
			"DatabaseService",
			{
				{ "Login" , "" },
				{ "Password" , "" },
				{ "FolderBackup" , "" },
				{ "FolderPostgresBin" , "" },
				{ "KeepOverDays", "" }
			}
		}
	};
}
//-----------------------------------------------------------------------------
ISConfig::~ISConfig()
{
	if (Settings)
	{
		delete Settings;
	}
}
//-----------------------------------------------------------------------------
ISConfig& ISConfig::GetInstance()
{
	static ISConfig Config;
	return Config;
}
//-----------------------------------------------------------------------------
void ISConfig::Initialize(const QString &config_file_path)
{
	ConfigFilePath = config_file_path;
	if (!QFile::exists(ConfigFilePath))
	{
		Generate();
	}

	Settings = new QSettings(ConfigFilePath, QSettings::IniFormat);
}
//-----------------------------------------------------------------------------
QVariant ISConfig::GetValue(const QString &ParameterName)
{
	IS_ASSERT(Settings, "Config not initialized");
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	return Settings->value(ParameterName);
}
//-----------------------------------------------------------------------------
bool ISConfig::GetValueBool(const QString &ParameterName)
{
	return GetValue(ParameterName).toBool();
}
//-----------------------------------------------------------------------------
QString ISConfig::GetValueString(const QString &ParameterName)
{
	return GetValue(ParameterName).toString();
}
//-----------------------------------------------------------------------------
int ISConfig::GetValueInt(const QString &ParameterName)
{
	return GetValue(ParameterName).toInt();
}
//-----------------------------------------------------------------------------
void ISConfig::SetValue(const QString &ParameterName, const QVariant &Value)
{
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	Settings->setValue(ParameterName, Value);
}
//-----------------------------------------------------------------------------
void ISConfig::ClearValue(const QString &ParameterName)
{
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found config key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	Settings->setValue(ParameterName, QVariant());
}
//-----------------------------------------------------------------------------
void ISConfig::Generate()
{
	//??? Вероятно, это временное решениие и тут будет использоваться класс CLSettings из монорепозитория
	Settings = new QSettings(ConfigFilePath, QSettings::IniFormat);
	for (const auto &SectionItem : Structure)
	{
		for (const auto &ParameterItem : SectionItem.second)
		{
			Settings->setValue(SectionItem.first + "/" + ParameterItem.first, ParameterItem.second);
		}
	}

	delete Settings;
	Settings = nullptr;
}
//-----------------------------------------------------------------------------
