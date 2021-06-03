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

    const std::string& GetErrorString() const;
    bool Init(const std::string &configuration_name, bool XSR, bool XSF);
    ISNamespace::FieldType GetType(const std::string &type);
    std::string GetTypeDB(ISNamespace::FieldType FieldType) const;
    PMetaTable* GetTable(const std::string &TableName);
    const std::vector<PMetaTable*>& GetTables() const;
    const std::vector<PMetaResource*>& GetResources() const;
    const std::vector<PMetaFunction*>& GetFunctions() const;
    std::vector<PMetaForeign*> GetForeigns() const;
    std::vector<PMetaIndex*> GetIndexes() const;
    const ISVectorString& GetVectorXSN() const;
    bool CheckExistResource(const std::string &UID) const;

private:
    bool XSNInit();
    bool XSNInit(const std::string &Content, size_t Size, const std::string &FileName, tinyxml2::XMLElement *XmlElementTemplateXNS);
    bool XSNInitTable(tinyxml2::XMLElement *XmlElement, tinyxml2::XMLElement *XmlElementTemplateXNS);
    bool XSNInitTableFields(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement);
    bool XSNInitIndexes(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement);
    bool XSNInitForeigns(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement);
    bool XSNInitEscorts(PMetaTable *MetaTable, tinyxml2::XMLElement *XmlElement);

    bool XSRInit();
    bool XSRInit(const std::string &Content, size_t Size, const std::string &FileName);
    bool XSRInit(tinyxml2::XMLElement *XmlElement);

    bool XSFInit();
    bool XSFInit(const std::string &Content, size_t Size, const std::string &FileName);
    bool XSFInit(tinyxml2::XMLElement *XmlElement);

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
    const char *CurrentXSN, *CurrentXSR, *CurrentXSF;
    std::vector<ISMetaType> VectorTypes; //Перечисление типов системы
    size_t TypesCount; //Количество типов

    //Списки файлов с мета-данными
    ISVectorString VectorFilesXSN, VectorFilesXSR, VectorFilesXSF;

    std::vector<PMetaTable*> Tables;
    std::vector<PMetaFunction*> Functions;
    std::vector<PMetaResource*> Resources;
};
//-----------------------------------------------------------------------------
#endif
