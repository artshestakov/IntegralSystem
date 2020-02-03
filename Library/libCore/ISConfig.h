#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISConfig
{
public:
	ISConfig(const ISConfig &) = delete;
	ISConfig(ISConfig &&) = delete;
	ISConfig &operator=(const ISConfig &) = delete;
	ISConfig &operator=(ISConfig &&) = delete;
	~ISConfig();

	static ISConfig& GetInstance();

	bool Initialize();
	
	QVariant GetValue(const QString &ParameterName); //Получить значение параметра
	bool GetValueBool(const QString &ParameterName);
	QString GetValueString(const QString &ParameterName);
	int GetValueInt(const QString &ParameterName);

	void SetValue(const QString &ParameterName, const QVariant &Value); //Изменить значение параметра
	void ClearValue(const QString &ParameterName); //Очистить значение параметра

private:
	bool Update();
	bool Create();

private:
	ISConfig();

	QString ErrorString;
	QSettings *Settings;
	QString PathConfigTemplate;
};
//-----------------------------------------------------------------------------
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::GetInstance().GetValue(PARAMETER_NAME)
#define CONFIG_BOOL(PARAMETER_NAME) ISConfig::GetInstance().GetValueBool(PARAMETER_NAME)
#define CONFIG_STRING(PARAMETER_NAME) ISConfig::GetInstance().GetValueString(PARAMETER_NAME)
#define CONFIG_INT(PARAMETER_NAME) ISConfig::GetInstance().GetValueInt(PARAMETER_NAME)
//-----------------------------------------------------------------------------
#endif
