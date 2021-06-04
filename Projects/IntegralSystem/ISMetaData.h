#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "ISStructsOld.h"
//-----------------------------------------------------------------------------
class ISMetaData
{
public:
    static ISMetaData& Instance();

    QString GetErrorString() const;
    bool Initialize(const QString &configuration_name, bool InitXSR, bool InitXSF); //�������������
	bool Initialize(const QVariantList &VariantList); //�������������

    PMetaTable* GetMetaTable(const QString &TableName); //�������� ����-������� �� �����
    PMetaField* GetMetaField(PMetaTable *MetaTable, const QString &FieldName); //�������� ����-���� �� ��������� �������
    PMetaField* GetMetaField(const QString &TableName, const QString &FieldName); //�������� ����-���� �� ��������� �������

    std::vector<PMetaFunction*> GetFunctions(); //�������� ������ ���� �������
    std::vector<PMetaTable*> GetTables(); //�������� ������ ���� ������
    std::vector<PMetaIndex*> GetIndexes(); //�������� ������ �������� ��� ���������������� �����
    std::vector<PMetaForeign*> GetForeigns(); //�������� ������ ������� ������
    std::vector<PMetaResource*> GetResources(); //�������� �������

    bool CheckExistTable(const QString &TableName) const; //��������� ������� ��������� ������� � ����
    bool CheckExitField(PMetaTable *MetaTable, const QString &FieldName) const; //��������� ������� ���������� ���� � ��������� �������
	bool CheckExistResource(const QString &UID) const; //��������� ������� ������� �� ��� ��������������

	const ISMetaType& GetType(const QString &type_name); //�������� ��� �� �����
	const ISMetaType& GetType(ISNamespace::FieldType type); //�������� ��� �� ����

protected:
    bool CheckUniqueAllIdentifiers(bool InitXSR); //�������� ������������ ���� ���������������
    bool CheckUniqueAllAliases(); //�������� ������������ ���� ����������� ������

    bool InitializeXSN(); //������������� XSN
    bool InitializeXSN(const QString &Content); //������������� �������� XSN

    bool InitializeXSNTable(QDomNode &DomNode); //������������� �������
    void InitializeXSNTableSystemFields(PMetaTable *MetaTable); //������������� ��������� ����� ��� �������
    bool InitializeXSNTableFields(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� ����� �������
    bool InitializeXSNTableIndexes(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� �������� ����� �������
    bool InitializeXSNTableForeigns(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� ������� ������ ����� �������
    bool InitializeXSNTableEscorts(PMetaTable *MetaTable, const QDomNode &DomNode); //������������� �������� �������

    bool InitializeXSR(); //������������� XSR
    bool InitializeXSR(const QString &Content); //������������� ����� XSR

    bool InitializeXSF(); //������������ XSF
    bool InitializeXSF(const QString &Content); //������������� ����� XSF

private:
    ISMetaData();
    ~ISMetaData();
	ISMetaData(const ISMetaData &) = delete;
	ISMetaData(ISMetaData &&) = delete;
	ISMetaData& operator=(const ISMetaData &) = delete;
	ISMetaData& operator=(ISMetaData &&) = delete;

private:
    QString ErrorString;
    QString ConfigurationName;

    QDomNode GetChildDomNode(QDomNode &TableNode, const QString &TagName) const;

    std::map<QString, PMetaFunction *> FunctionsMap; //�������
    std::map<QString, PMetaTable *> TablesMap; //�������
    std::vector<PMetaResource *> Resources; //�������

    QString CurrentXSN; //������� �������������� XSN
    QString CurrentXSR; //������� �������������� XSR
    QString CurrentXSF; //������� �������������� XSF
    bool Initialized; //���� �������������
    std::vector<ISMetaType> VectorTypes; //������������ ����� �������

	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#endif
