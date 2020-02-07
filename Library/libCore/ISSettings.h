#pragma once
#ifndef _ISSETTINGS_H_INCLUDED
#define _ISSETTINGS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISSettings
{
public:
	ISSettings(const ISSettings &) = delete;
	ISSettings(ISSettings &&) = delete;
	ISSettings &operator=(const ISSettings &) = delete;
	ISSettings &operator=(ISSettings &&) = delete;
	~ISSettings();

	static ISSettings& GetInstance();

	bool GetValueBool(const QString &SettingUID);
	QString GetValueString(const QString &SettingUID);
	int GetValueInt(const QString &SettingUID);
	
	QVariant GetValue(const QString &SettingUID); //Получить значение мета-настройки
	void SetValue(const QString &SettingUID, const QVariant &Value); //Изменить значение мета-настройки
	
	QVector<ISMetaSettingsGroup*> GetSettingGroups(); //Получить все группы настроек
	ISMetaSetting* GetMetaSetting(const QString &SettingUID); //Получить мета-настройку по её идентификатору
	
	void SaveValue(const QString &SettingUID, const QVariant &Value);

protected:
	void Initialize(); //Инициализация
	ISMetaSettingsGroup* CheckExistGroup(const ISUuid &GroupUID);
	void InsertSetting(const QString &SettingUID, const QVariant &Value);

private:
	ISSettings();

	QVector<ISMetaSettingsGroup*> SettingGroups;
};
//-----------------------------------------------------------------------------
#define SETTING_VALUE(SETTING_UID) ISSettings::GetInstance().GetValue(SETTING_UID)
#define SETTING_BOOL(SETTING_UID) ISSettings::GetInstance().GetValueBool(SETTING_UID)
#define SETTING_STRING(SETTING_UID) ISSettings::GetInstance().GetValueString(SETTING_UID)
#define SETTING_INT(SETTING_UID) ISSettings::GetInstance().GetValueInt(SETTING_UID)
//-----------------------------------------------------------------------------
#endif
