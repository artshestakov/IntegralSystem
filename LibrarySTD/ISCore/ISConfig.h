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
	typedef std::map<std::string, std::map<std::string, ISConfigParameter>> TemplateMap;

public:
	static ISConfig& Instance();

	std::string GetErrorString() const;
	bool IsValid(); //��������� ������������ ���������� ����������������� �����
	bool Initialize(ISNamespace::ConfigType Type);
	
	//������� ��������� ��������
	std::string GetValueString(const std::string &SectionName, const std::string &ParameterName);
	int GetValueInt(const std::string &SectionName, const std::string &ParameterName);
	bool GetValueBool(const std::string &SectionName, const std::string &ParameterName);

private:
	std::string GetValue(const std::string &SectionName, const std::string &ParameterName); //�������� �������� ���������
	bool Update(); //���������� �����
	bool Create(); //��������� ����� �� �������

private:
	ISConfig::TemplateMap GetTemplate(ISNamespace::ConfigType Type) const;

private:
	ISConfig();
	~ISConfig();
	ISConfig(const ISConfig&) = delete;
	ISConfig(ISConfig&&) = delete;
	ISConfig& operator=(const ISConfig&) = delete;
	ISConfig& operator=(ISConfig&&) = delete;

private:
	ISConfig::TemplateMap TemplateServer;
	ISConfig::TemplateMap TemplateClient;

private:
	ISConfig::TemplateMap MapConfig;
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
