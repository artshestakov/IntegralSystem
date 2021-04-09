#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include "tinyxml2.h"
#include "PMetaClass.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISMetaData
{
public:
    static ISMetaData& Instance();

    std::string GetErrorString() const;
    bool Init(const std::string &configuration_name, bool XSR, bool XSF);
    ISNamespace::FieldType GetType(const std::string &type);

private:
    bool InitXSN();
    bool InitXSN(const std::string &Content, size_t Size, const std::string &FileName);
    bool InitXSNTable(tinyxml2::XMLElement *XmlElement);
    bool InitXSNTableSystemFields(PMetaTable *MetaTable);
    bool InitXSNTableFields(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement);

    bool InitXSR();
    bool InitXSF();

private:
    ISMetaData();
    ~ISMetaData();
    ISMetaData(const ISMetaData&) = delete;
    ISMetaData(ISMetaData&&) = delete;
    ISMetaData& operator=(const ISMetaData&) = delete;
    ISMetaData& operator=(ISMetaData&&) = delete;

private:
    std::string ErrorString;
    ISCriticalSection CS;
    bool Initialized;
    std::string ConfigurationName;
    const char *CurrentXSN;
    std::vector<ISMetaType> VectorTypes; //Перечисление типов системы
    size_t TypesCount; //Количество типов
    std::vector<std::string> VectorFilesXSN;

    std::vector<PMetaTable*> Tables;
};
//-----------------------------------------------------------------------------
#endif
