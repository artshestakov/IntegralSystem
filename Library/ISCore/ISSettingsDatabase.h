#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISSettingsDatabase
{
public:
	static ISSettingsDatabase& Instance();

	QString GetErrorString() const;
	void Initialize(const QVariantMap &VariantMap);
	bool Initialize(); //Инициализация настроек базы данных
	QVariant GetValue(const QString &SettingName); //Получить значение настройки
	void SetValue(const QString &SettingName, const QVariant &Value); //Изменить значение настройки

private:
	ISSettingsDatabase();
	~ISSettingsDatabase();
	ISSettingsDatabase(ISSettingsDatabase const &) {};
	ISSettingsDatabase& operator=(ISSettingsDatabase const&) { return *this; };

private:
	QString ErrorString;
	ISStringToVariantMap Settings;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#define SETTING_DATABASE_VALUE(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME)
#define SETTING_DATABASE_VALUE_BOOL(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME).toBool()
#define SETTING_DATABASE_VALUE_STRING(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME).toString()
#define SETTING_DATABASE_VALUE_INT(SETTING_DATABASE_NAME) ISSettingsDatabase::Instance().GetValue(SETTING_DATABASE_NAME).toInt()
//-----------------------------------------------------------------------------
