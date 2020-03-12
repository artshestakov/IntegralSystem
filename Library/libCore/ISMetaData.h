#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClass.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaData
{
public:
	ISMetaData(const ISMetaData &) = delete;
	ISMetaData(ISMetaData &&) = delete;
	ISMetaData &operator=(const ISMetaData &) = delete;
	ISMetaData &operator=(ISMetaData &&) = delete;
	~ISMetaData();

	static ISMetaData& GetInstanse();

	QString GetErrorString() const;
	bool Initialize(const QString &configuration_name, bool InitXSR, bool InitXSF); //�������������

	PMetaClassTable* GetMetaTable(const QString &TableName); //�������� ����-������� �� �����
	PMetaClassTable* GetMetaQuery(const QString &QueryName); //�������� ����-������ �� �����
	PMetaClassField* GetMetaField(PMetaClassTable *MetaTable, const QString &FieldName); //�������� ����-���� �� ��������� �������
	PMetaClassField* GetMetaField(const QString &TableName, const QString &FieldName); //�������� ����-���� �� ��������� �������

	std::vector<PMetaClassFunction*> GetFunctions(); //�������� ������ ���� �������
	std::vector<PMetaClassTable*> GetTables(); //�������� ������ ���� ������
	ISVectorString GetMetaQueries(); //�������� ������ ���� ����-��������
	std::vector<PMetaClassIndex*> GetSystemIndexes(); //�������� ������ ��������� ��������
	std::vector<PMetaClassIndex*> GetIndexes(); //�������� ������ �������� ��� ���������������� �����
	std::vector<PMetaClassIndex*> GetCompoundIndexes(); //�������� ������ ��������� ��������
	std::vector<PMetaClassForeign*> GetForeigns(); //�������� ������ ������� ������
	std::vector<PMetaClassResource*> GetResources(); //�������� �������

	bool CheckExistTable(const QString &TableName) const; //��������� ������� ��������� ������� � ����
	bool CheckExitField(PMetaClassTable *MetaTable, const QString &FieldName) const; //��������� ������� ���������� ���� � ��������� �������

	ISNamespace::FieldType GetTypeField(const QString &type_name); //�������� ��� ������ �� ����-�����
	QString GetTypeDB(ISNamespace::FieldType field_type) const; //�������� ������������ ���� � ���� ������
	QString GetTypeDB(const QString &type_name) const; //�������� ������������ ���� � ���� ������ �� ����� ���� � �������
	QString GetNameFieldEdit(ISNamespace::FieldType field_type) const; //�������� ������������ ������ ��� ���� ������
	QString GetSearchWidget(ISNamespace::FieldType field_type) const; //�������� ��� ���������� �������
	bool GetSearch(ISNamespace::FieldType field_type) const; //�������� ���� ���������� �� ����� ��� ���� ������

protected:
	bool CheckUniqueAllIdentifiers(bool InitXSR); //�������� ������������ ���� ���������������
	bool CheckUniqueAllAliases(); //�������� ������������ ���� ����������� ������
	bool GenerateSqlFromForeigns(); //��������� SQL-�������� ��� ������� ������

	bool InitializeXSN(); //������������� XSN
	bool InitializeXSN(const QString &Content); //������������� �������� XSN
	
	bool InitializeXSNTable(QDomNode &DomNode); //������������� �������
	void InitializeXSNTableSystemFields(PMetaClassTable *MetaTable); //������������� ��������� ����� ��� �������
    bool InitializeXSNTableSystemFieldsVisible(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� ��������� ��������� �����
    bool InitializeXSNTableFields(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� ����� �������
    bool InitializeXSNTableIndexes(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� �������� ����� �������
    bool InitializeXSNTableForeigns(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� ������� ������ ����� �������
    bool InitializeXSNTableEscorts(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� �������� �������
    bool InitializeXSNTableJoins(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� JOIN'��

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

	std::map<QString, PMetaClassFunction *> FunctionsMap; //�������
	std::map<QString, PMetaClassTable *> TablesMap; //�������
	std::map<QString, PMetaClassTable *> QueriesMap; //�������

	std::vector<PMetaClassResource *> Resources; //������
	std::vector<PMetaClassIndex*> IndexesCompound; //��������� �������

	QString CurrentXSN; //������� �������������� XSN
	QString CurrentXSR; //������� �������������� XSR
	QString CurrentXSF; //������� �������������� XSF
	bool Initialized; //���� �������������
	std::vector<ISMetaDataType> VectorTypes; //������������ ����� �������
};
//-----------------------------------------------------------------------------
#endif
