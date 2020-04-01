#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISConfig
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
//�������� �������� ��������� �� ����������������� �����
#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME)

//�������� �������� ���� bool
#define CONFIG_BOOL(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toBool()

//�������� �������� ���� QString
#define CONFIG_STRING(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toString()

//�������� �������� ���� int
#define CONFIG_INT(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toInt()
//-----------------------------------------------------------------------------
#endif
