#pragma once
#ifndef _ISCONFIG_H_INCLUDED
#define _ISCONFIG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
#include "ISTypedefs.h"
#include "SimpleIni.h"
//-----------------------------------------------------------------------------
class ISConfig
{
public:
    static ISConfig& Instance();

    const std::string& GetErrorString() const;
    bool IsValid(); //Проверить корректность заполнения конфигурационного файла
    bool IsEmpty(const std::string &SectionName, const std::string &ParameterName);
    bool Initialize(const std::string &template_name);
    
    //Функции получения значений
    std::string GetValueString(const std::string &SectionName, const std::string &ParameterName);
    int GetValueInt(const std::string &SectionName, const std::string &ParameterName);
    unsigned short GetValueUShort(const std::string &SectionName, const std::string &ParameterName);
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
    CSimpleIni SimpleINI;
    ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#endif
