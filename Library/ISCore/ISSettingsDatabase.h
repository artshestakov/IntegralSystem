#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISSettingsDatabase
{
public:
	ISSettingsDatabase(const ISSettingsDatabase &) = delete;
	ISSettingsDatabase(ISSettingsDatabase &&) = delete;
	ISSettingsDatabase &operator=(const ISSettingsDatabase &) = delete;
	ISSettingsDatabase &operator=(ISSettingsDatabase &&) = delete;
	~ISSettingsDatabase();

	static ISSettingsDatabase& GetInstance();

	void Initialize(); //Инициализация настроек базы данных

	QVariant GetValueDB(const QString &SettingName); //Получить значение настройки из базы
	QVariant GetValue(const QString &SettingName); //Получить значение настройки
	QVariant GetSystemParameter(const ISUuid &UID); //Получить значение системного параметра

private:
	ISSettingsDatabase();

	ISStringToVariantMap Settings;
	std::map<ISUuid, QVariant> SystemParameters;
};
//-----------------------------------------------------------------------------
#define SETTING_DATABASE_VALUE_DB(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValueDB(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValue(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE_BOOL(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValue(SETTING_DATABASE_NAME).toBool()
#define SETTING_DATABASE_VALUE_STRING(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValue(SETTING_DATABASE_NAME).toString()
#define SETTING_DATABASE_VALUE_INT(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValue(SETTING_DATABASE_NAME).toInt()
//-----------------------------------------------------------------------------
