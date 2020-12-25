#include "ISSettingsDatabase.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISSettingsDatabase::ISSettingsDatabase()
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISSettingsDatabase::~ISSettingsDatabase()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISSettingsDatabase& ISSettingsDatabase::Instance()
{
	static ISSettingsDatabase SettingsDatabase;
	return SettingsDatabase;
}
//-----------------------------------------------------------------------------
void ISSettingsDatabase::Initialize(const QVariantMap &VariantMap)
{
	for (const auto &MapItem : VariantMap.toStdMap())
	{
		Settings[MapItem.first] = MapItem.second;
	}
}
//-----------------------------------------------------------------------------
QVariant ISSettingsDatabase::GetValue(const QString &SettingName)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	QVariant Value = Settings[SettingName];
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return Value;
}
//-----------------------------------------------------------------------------
void ISSettingsDatabase::SetValue(const QString &SettingName, const QVariant &Value)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	Settings[SettingName] = Value;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
