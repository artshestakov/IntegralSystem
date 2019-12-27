#include "StdAfx.h"
#include "ISConfig.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISDebug.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: Settings(nullptr)
{
	Settings = nullptr;
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
void ISConfig::Initialize(const QString &ConfigFilePath)
{
	IS_ASSERT(QFile::exists(ConfigFilePath), QString("Not found file config: %1").arg(ConfigFilePath));
	Settings = new QSettings(ConfigFilePath, QSettings::IniFormat);
	IS_ASSERT(Settings->allKeys().count(), QString("Config file \"%1\" is empty.").arg(ConfigFilePath));

	ISDebug::ShowInfoString(QString("Config initialized. Server: %1, Port: %2, Database: %3").arg(GetValue(CONST_CONFIG_CONNECTION_SERVER).toString()).arg(GetValue(CONST_CONFIG_CONNECTION_PORT).toInt()).arg(GetValue(CONST_CONFIG_CONNECTION_DATABASE).toString()));
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
