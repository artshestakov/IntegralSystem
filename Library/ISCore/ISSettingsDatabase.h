#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISUuid.h"
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
	void InitializedSystemParameters(); //Инициализация системных параметров базы данных

	bool GetValueBool(const QString &SettingName);
	QString GetValueString(const QString &SettingName);
	int GetValueInt(const QString &SettingName);

	QVariant GetValueDB(const QString &SettingName); //Получить значение настройки из базы
	QVariant GetValue(const QString &SettingName); //Получить значение настройки
	QVariant GetSystemParameter(const ISUuid &UID); //Получить значение системного параметра

private:
	ISSettingsDatabase();

	QMap<QString, QVariant> Settings;
	QMap<ISUuid, QVariant> SystemParameters;
};
//-----------------------------------------------------------------------------
#define SETTING_DATABASE_VALUE_DB(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValueDB(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValue(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE_BOOL(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValueBool(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE_STRING(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValueString(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE_INT(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValueInt(SETTING_DATABASE_NAME)
//-----------------------------------------------------------------------------
