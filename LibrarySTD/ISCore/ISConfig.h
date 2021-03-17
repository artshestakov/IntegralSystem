#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
#include "ISTypedefs.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISConfig
{
public:
	static ISConfig& Instance();

	std::string GetErrorString() const;
	bool IsValid(); //��������� ������������ ���������� ����������������� �����
	bool Initialize(ISNamespace::ConfigType Type);
	std::string GetValue(const std::string &ParameterName); //�������� �������� ���������

private:
	bool Update(); //���������� �����
	bool Create(ISNamespace::ConfigType Type); //��������� ����� �� �������

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
	std::map<std::string, std::map<std::string, ISConfigParameter>> TemplateServer;
	std::map<std::string, std::map<std::string, ISConfigParameter>> TemplateClient;
	std::string ErrorString;
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
