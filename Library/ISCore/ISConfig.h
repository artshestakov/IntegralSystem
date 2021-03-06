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
    bool IsValid(); //��������� ������������ ���������� ����������������� �����
    bool IsEmpty(const std::string &SectionName, const std::string &ParameterName);
    bool Initialize(const std::string &template_name);
    
    //������� ��������� ��������
    std::string GetString(const std::string &SectionName, const std::string &ParameterName);
    const char* GetCString(const std::string &SectionName, const std::string &ParameterName);
    int GetInt(const std::string &SectionName, const std::string &ParameterName);
    unsigned short GetUShort(const std::string &SectionName, const std::string &ParameterName);
    bool GetBool(const std::string &SectionName, const std::string &ParameterName);

    //������� ��������� ��������
    void SetString(const std::string &SectionName, const std::string &ParameterName, const std::string &Value);
    void SetUShort(const std::string &SectionName, const std::string &ParameterName, unsigned short Value);
    void SetBool(const std::string &SectionName, const std::string &ParameterName, bool Value);

private:
    void SetValue(const std::string &SectionName, const std::string &ParameterName, const std::string &Value);
    std::string GetValue(const std::string &SectionName, const std::string &ParameterName); //�������� �������� ���������
    bool Update(); //���������� �����
    bool Create(); //��������� ����� �� �������

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
