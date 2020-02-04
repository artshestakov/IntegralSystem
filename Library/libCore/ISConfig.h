#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISConfig
{
public:
	ISConfig(const ISConfig &) = delete;
	ISConfig(ISConfig &&) = delete;
	ISConfig &operator=(const ISConfig &) = delete;
	ISConfig &operator=(ISConfig &&) = delete;
	~ISConfig();

	static ISConfig& GetInstance();

	void Initialize();
	
	QVariant GetValue(const QString &ParameterName); //�������� �������� ���������
	void SetValue(const QString &ParameterName, const QVariant &Value); //�������� �������� ���������

private:
	void Update(); //���������� �����
	void Create(); //��������� ����� �� �������

private:
	ISConfig();

	QString ErrorString;
	QSettings *Settings;
	QString PathConfigTemplate;
};
//-----------------------------------------------------------------------------
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::GetInstance().GetValue(PARAMETER_NAME)
#define CONFIG_BOOL(PARAMETER_NAME) ISConfig::GetInstance().GetValue(PARAMETER_NAME).toBool()
#define CONFIG_STRING(PARAMETER_NAME) ISConfig::GetInstance().GetValue(PARAMETER_NAME).toString()
#define CONFIG_INT(PARAMETER_NAME) ISConfig::GetInstance().GetValue(PARAMETER_NAME).toInt()
//-----------------------------------------------------------------------------
#endif
