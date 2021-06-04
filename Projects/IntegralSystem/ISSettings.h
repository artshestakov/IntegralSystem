#pragma once
#ifndef _ISSETTINGS_H_INCLUDED
#define _ISSETTINGS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISSettings
{
public:
	static ISSettings& Instance();
	
	void Initialize(const QVariantList &VariantList);
	QVariantMap GetSettingsChanged() const; //�������� ���������� ���������

	QVariant GetValue(const QString &SettingUID); //�������� �������� ����-���������
	void SetValue(const QString &SettingUID, const QVariant &Value); //�������� �������� ����-���������
	
	std::vector<ISMetaSettingsGroup*> GetSettingGroups(); //�������� ��� ������ ��������
	ISMetaSetting* GetMetaSetting(const QString &SettingUID); //�������� ����-��������� �� � ��������������

private:
	ISSettings();
	~ISSettings();
	ISSettings(const ISSettings&) = delete;
	ISSettings(ISSettings&&) = delete;
	ISSettings& operator=(const ISSettings&) = delete;
	ISSettings& operator=(ISSettings&&) = delete;

private:
	std::vector<ISMetaSettingsGroup*> SettingGroups;
	QVariantMap SettingsChanged; //���������� � ���� ������ ��������� ���������, ����� ��� ����������
};
//-----------------------------------------------------------------------------
#define SETTING_VALUE(SETTING_UID) ISSettings::Instance().GetValue(SETTING_UID)
#define SETTING_BOOL(SETTING_UID) ISSettings::Instance().GetValue(SETTING_UID).toBool()
#define SETTING_STRING(SETTING_UID) ISSettings::Instance().GetValue(SETTING_UID).toString()
#define SETTING_INT(SETTING_UID) ISSettings::Instance().GetValue(SETTING_UID).toInt()
//-----------------------------------------------------------------------------
#endif
