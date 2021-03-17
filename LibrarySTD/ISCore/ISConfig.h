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
	bool IsValid(); //Проверить корректность заполнения конфигурационного файла
	bool Initialize(const std::string &template_name);
	
	//Функции получения значений
	std::string GetValueString(const std::string &SectionName, const std::string &ParameterName);
	int GetValueInt(const std::string &SectionName, const std::string &ParameterName);
	bool GetValueBool(const std::string &SectionName, const std::string &ParameterName);

private:
	std::string GetValue(const std::string &SectionName, const std::string &ParameterName); //Получить значение параметра
	bool Update(); //Обновление файла
	bool Create(); //Генерация файла из шаблона

private:
	bool ContainsKey(const std::string &SectionName, const std::string &ParameterName);

private:
	ISConfig();
	~ISConfig();
	ISConfig(const ISConfig&) = delete;
	ISConfig(ISConfig&&) = delete;
	ISConfig& operator=(const ISConfig&) = delete;
	ISConfig& operator=(ISConfig&&) = delete;

private:
	std::string TemplateName;
	std::vector<ISConfigParameter> VectorTemplate;
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
