#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISConfig
{
public:
	static ISConfig& Instance();

	std::string GetErrorString() const;
	bool IsValid(); //��������� ������������ ���������� ����������������� �����
	bool Initialize(const std::string &template_name);
	std::string GetValue(const std::string &ParameterName); //�������� �������� ���������

private:
	bool Update(); //���������� �����
	bool Create(); //��������� ����� �� �������

private:
	bool ContainsKey(const std::string &Key); //��������� ������� ����� � �������
	std::string GetDefaultValue(const std::string &Key) const; //�������� �������� �� ��������� ��� ���������

private:
	ISConfig();
	~ISConfig();
	ISConfig(const ISConfig&) = delete;
	ISConfig(ISConfig&&) = delete;
	ISConfig& operator=(const ISConfig&) = delete;
	ISConfig& operator=(ISConfig&&) = delete;

private:
	std::map<std::string, std::map<std::string, std::vector<ISConfigParameter>>> MapTemplate;
	std::map<std::string, std::string> MapValues;
	std::string ErrorString;
	std::string TemplateName;
	std::string PathConfigFile;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
//#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME) //�������� �������� ��������� �� ����������������� �����
//#define CONFIG_BOOL(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toBool() //�������� �������� ���� bool
//#define CONFIG_STRING(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toString() //�������� �������� ���� QString
//#define CONFIG_INT(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toInt() //�������� �������� ���� int
//-----------------------------------------------------------------------------
#endif
