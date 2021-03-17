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
#define CONFIG_BOOL(SECTION_NAME, PARAMETER_NAME) ISConfig::Instance().GetValueBool(SECTION_NAME, PARAMETER_NAME)
#define CONFIG_STRING(SECTION_NAME, PARAMETER_NAME) ISConfig::Instance().GetValueString(SECTION_NAME, PARAMETER_NAME)
#define CONFIG_INT(SECTION_NAME, PARAMETER_NAME) ISConfig::Instance().GetValueInt(SECTION_NAME, PARAMETER_NAME)
//-----------------------------------------------------------------------------
#endif
