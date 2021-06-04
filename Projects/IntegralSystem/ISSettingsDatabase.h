#pragma once
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISSettingsDatabase
{
public:
	static ISSettingsDatabase& Instance();

	void Initialize(const QVariantMap &VariantMap);
	QVariant GetValue(const QString &SettingName); //Получить значение настройки
	void SetValue(const QString &SettingName, const QVariant &Value); //Изменить значение настройки

private:
	ISSettingsDatabase();
	~ISSettingsDatabase();
	ISSettingsDatabase(const ISSettingsDatabase&) = delete;
	ISSettingsDatabase(ISSettingsDatabase&&) = delete;
	ISSettingsDatabase& operator=(const ISSettingsDatabase&) = delete;
	ISSettingsDatabase& operator=(ISSettingsDatabase&&) = delete;

private:
	ISStringToVariantMap Settings;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define SETTING_DATABASE_VALUE(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE_BOOL(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME).toBool()
#define SETTING_DATABASE_VALUE_STRING(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME).toString()
#define SETTING_DATABASE_VALUE_INT(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME).toInt()
//-----------------------------------------------------------------------------
