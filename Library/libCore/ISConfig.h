#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISConfig
{
public:
	static ISConfig& Instance();

	QString GetErrorString() const;
	bool Initialize();
	
	QVariant GetValue(const QString &ParameterName); //�������� �������� ���������
	void SetValue(const QString &ParameterName, const QVariant &Value); //�������� �������� ���������
	void SaveForce(); //�������������� ����������

private:
	bool Update(); //���������� �����
	bool Create(); //��������� ����� �� �������

private:
	ISConfig();
	~ISConfig();
	ISConfig(ISConfig const &) {};
	ISConfig& operator=(ISConfig const&) { return *this; };

private:
	QString ErrorString;
	QSettings *Settings;
	QString PathConfigTemplate;
};
//-----------------------------------------------------------------------------
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME)
#define CONFIG_BOOL(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME).toBool()
#define CONFIG_STRING(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME).toString()
#define CONFIG_INT(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME).toInt()
//-----------------------------------------------------------------------------
#endif
