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
	bool IsValid(); //Проверить корректность заполнения конфигурационного файла
	bool Initialize(ISNamespace::ConfigType Type);
	std::string GetValue(const std::string &ParameterName); //Получить значение параметра

private:
	bool Update(); //Обновление файла
	bool Create(ISNamespace::ConfigType Type); //Генерация файла из шаблона

private:
	bool ContainsKey(const std::string &Key); //Проверить наличие ключа в шаблоне
	std::string GetDefaultValue(const std::string &Key) const; //Получить значение по умолчанию для параметра

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
//#define CONFIG_VALUE(PARAMETER_NAME) ISConfig::Instance().GetValue(PARAMETER_NAME) //Получить значение параметра из конфигурационного файла
//#define CONFIG_BOOL(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toBool() //Получить значение типа bool
//#define CONFIG_STRING(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toString() //Получить значение типа QString
//#define CONFIG_INT(PARAMETER_NAME) CONFIG_VALUE(PARAMETER_NAME).toInt() //Получить значение типа int
//-----------------------------------------------------------------------------
#endif
