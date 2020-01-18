#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISSettingsDatabase
{
public:
	ISSettingsDatabase(const ISSettingsDatabase &) = delete;
	ISSettingsDatabase(ISSettingsDatabase &&) = delete;
	ISSettingsDatabase &operator=(const ISSettingsDatabase &) = delete;
	ISSettingsDatabase &operator=(ISSettingsDatabase &&) = delete;
	~ISSettingsDatabase();

	static ISSettingsDatabase& GetInstance();

	void Initialize(); //������������� �������� ���� ������
	void InitializedSystemParameters(); //������������� ��������� ���������� ���� ������

	bool GetValueBool(const QString &SettingName);
	QString GetValueString(const QString &SettingName);
	int GetValueInt(const QString &SettingName);
	QColor GetValueColor(const QString &SettingName);

	QVariant GetValueDB(const QString &SettingName); //�������� �������� ��������� �� ����
	QVariant GetValue(const QString &SettingName); //�������� �������� ���������
	QVariant GetSystemParameter(const ISUuid &UID); //�������� �������� ���������� ���������

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
#define SETTING_DATABASE_VALUE_COLOR(SETTING_DATABASE_NAME) ISSettingsDatabase::GetInstance().GetValueColor(SETTING_DATABASE_NAME)
//-----------------------------------------------------------------------------
