#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaData
{
public:
    ISMetaData(const ISMetaData &) = delete;
    ISMetaData(ISMetaData &&) = delete;
    ISMetaData &operator=(const ISMetaData &) = delete;
    ISMetaData &operator=(ISMetaData &&) = delete;
    ~ISMetaData();

    static ISMetaData& Instance();

    QString GetErrorString() const;
    bool Initialize(const QString &configuration_name, bool InitXSR, bool InitXSF); //�������������

    PMetaTable* GetMetaTable(const QString &TableName); //�������� ����-������� �� �����
    PMetaTable* GetMetaQuery(const QString &QueryName); //�������� ����-������ �� �����
    PMetaField* GetMetaField(PMetaTable *MetaTable, const QString &FieldName); //�������� ����-���� �� ��������� �������
    PMetaField* GetMetaField(const QString &TableName, const QString &FieldName); //�������� ����-���� �� ��������� �������

    std::vector<PMetaFunction*> GetFunctions(); //�������� ������ ���� �������
    std::vector<PMetaTable*> GetTables(); //�������� ������ ���� ������
    std::vector<PMetaIndex*> GetSystemIndexes(); //�������� ������ ��������� ��������
    std::vector<PMetaIndex*> GetIndexes(); //�������� ������ �������� ��� ���������������� �����
    std::vector<PMetaIndex*> GetCompoundIndexes(); //�������� ������ ��������� ��������
    std::vector<PMetaForeign*> GetForeigns(); //�������� ������ ������� ������
    std::vector<PMetaResource*> GetResources(); //�������� �������

    bool CheckExistTable(const QString &TableName) const; //��������� ������� ��������� ������� � ����
    bool CheckExitField(PMetaTable *MetaTable, const QString &FieldName) const; //��������� ������� ���������� ���� � ��������� �������
	bool CheckExistResource(const ISUuid &ResourceUID) const; //��������� ������� ������� �� ��� ��������������

    ISNamespace::FieldType GetTypeField(const QString &type_name); //�������� ��� ������ �� ����-�����
    QString GetTypeDB(ISNamespace::FieldType field_type) const; //�������� ������������ ���� � ���� ������
    QString GetTypeDB(const QString &type_name) const; //�������� ������������ ���� � ���� ������ �� ����� ���� � �������
    QString GetNameFieldEdit(ISNamespace::FieldType field_type) const; //�������� ������������ ������ ���� �������������� ��� ���� ������
    QString GetSearchWidget(ISNamespace::FieldType field_type) const; //�������� ��� ���������� �������
    bool GetSearch(ISNamespace::FieldType field_type) const; //�������� ���� ���������� �� ����� ��� ���� ������

protected:
    bool CheckUniqueAllIdentifiers(bool InitXSR); //�������� ������������ ���� ���������������
    bool CheckUniqueAllAliases(); //�������� ������������ ���� ����������� ������
    bool GenerateSqlFromForeigns(); //��������� SQL-�������� ��� ������� ������

    bool InitializeXSN(); //������������� XSN
    bool InitializeXSN(const QString &Content); //������������� �������� XSN

    bool InitializeXSNTable(QDomNode &DomNode); //������������� �������
    void InitializeXSNTableSystemFields(PMetaTable *MetaTable); //������������� ��������� ����� ��� �������
    bool InitializeXSNTableSystemFieldsVisible(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� ��������� ��������� �����
    bool InitializeXSNTableFields(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� ����� �������
    bool InitializeXSNTableIndexes(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� �������� ����� �������
    bool InitializeXSNTableForeigns(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� ������� ������ ����� �������
    bool InitializeXSNTableEscorts(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� �������� �������
    bool InitializeXSNTableJoins(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� JOIN'��

    bool InitializeXSR(); //������������� XSR
    bool InitializeXSR(const QString &Content); //������������� ����� XSR

    bool InitializeXSF(); //������������ XSF
    bool InitializeXSF(const QString &Content); //������������� ����� XSF

private:
    ISMetaData();

private:
    QString ErrorString;
    QString ConfigurationName;

    QDomNode GetChildDomNode(QDomNode &TableNode, const QString &TagName) const;

    std::map<QString, PMetaFunction *> FunctionsMap; //�������
    std::map<QString, PMetaTable *> TablesMap; //�������
    std::map<QString, PMetaTable *> QueriesMap; //�������

    std::vector<PMetaResource *> Resources; //������
    std::vector<PMetaIndex*> IndexesCompound; //��������� �������

    QString CurrentXSN; //������� �������������� XSN
    QString CurrentXSR; //������� �������������� XSR
    QString CurrentXSF; //������� �������������� XSF
    bool Initialized; //���� �������������
    std::vector<ISMetaDataType> VectorTypes; //������������ ����� �������
};
//-----------------------------------------------------------------------------
#endif
