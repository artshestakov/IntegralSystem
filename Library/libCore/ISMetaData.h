#pragma once
#ifndef _ISMETADATA_H_INCLUDED
#define _ISMETADATA_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClassFunction.h"
#include "PMetaClassTable.h"
#include "PMetaClassQuery.h"
#include "PMetaClassResource.h"
#include "ISAssociationTypes.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaData : public QObject
{
	Q_OBJECT

public:
	ISMetaData(const ISMetaData &) = delete;
	ISMetaData(ISMetaData &&) = delete;
	ISMetaData &operator=(const ISMetaData &) = delete;
	ISMetaData &operator=(ISMetaData &&) = delete;
	~ISMetaData();

	static ISMetaData& GetInstanse();

	bool GetInitialized() const;
	void Initialize(const QString &configuration, bool InitXSR, bool InitXSF); //�������������

	PMetaClassTable* GetMetaTable(const QString &TableName); //�������� ����-������� �� �����
	PMetaClassTable* GetMetaQuery(const QString &QueryName); //�������� ����-������ �� �����
	PMetaClassField* GetMetaField(PMetaClassTable *MetaTable, const QString &FieldName); //�������� ����-���� �� ��������� �������
	PMetaClassField* GetMetaField(const QString &TableName, const QString &FieldName); //�������� ����-���� �� ��������� �������

	QList<PMetaClassFunction*> GetFunctions(); //�������� ������ ���� �������
	QList<PMetaClassTable*> GetTables(); //�������� ������ ���� ������
	QVectorString GetMetaQueries(); //�������� ������ ���� ����-��������
	QVector<PMetaClassIndex*> GetSystemIndexes(); //�������� ������ ��������� ��������
	QVector<PMetaClassIndex*> GetIndexes(); //�������� ������ �������� ��� ���������������� �����
	QVector<PMetaClassIndex*> GetCompoundIndexes(); //�������� ������ ��������� ��������
	QVector<PMetaClassForeign*> GetForeigns(); //�������� ������ ������� ������
	QVector<PMetaClassResource*> GetResources(); //�������� �������

	bool CheckExistTable(const QString &TableName) const; //��������� ������� ��������� ������� � ����
	bool CheckExitField(PMetaClassTable *MetaTable, const QString &FieldName) const; //��������� ������� ���������� ���� � ��������� �������
	ISAssociationTypes& GetAssociationTypes(); //�������� ����������

protected:
	void CheckUniqueAllIdentifiers(bool InitXSR); //�������� ������������ ���� ���������������
	void CheckUniqueAllAliases(); //�������� ������������ ���� ����������� ������
	void GenerateSqlFromForeigns(); //��������� SQL-�������� ��� ������� ������

	void InitializeXSN(); //������������� XSN
	void InitializeXSN(const QString &Content); //������������� �������� XSN
	
	void InitializeXSNTable(QDomNode &DomNode); //������������� �������
	void InitializeXSNTableSystemFields(PMetaClassTable *MetaTable); //������������� ��������� ����� ��� �������
    void InitializeXSNTableSystemFieldsVisible(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� ��������� ��������� �����
    void InitializeXSNTableFields(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� ����� �������
    void InitializeXSNTableIndexes(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� �������� ����� �������
    void InitializeXSNTableForeigns(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� ������� ������ ����� �������
    void InitializeXSNTableEscorts(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� �������� �������
    void InitializeXSNTableJoins(PMetaClassTable *MetaTable, const QDomNode &DomNode); //������������� JOIN'��

	void InitializeXSR(); //������������� XSR
	void InitializeXSR(const QString &Content); //������������� ����� XSR

	void InitializeXSF(); //������������ XSF
	void InitializeXSF(const QString &Content); //������������� ����� XSF

private:
	ISMetaData();
	QString Configuration;

    QDomNode GetChildDomNode(QDomNode &TableNode, const QString &TagName) const;

	QMap<QString, PMetaClassFunction*> FunctionsMap; //�������
	QMap<QString, PMetaClassTable*> TablesMap; //�������
	QMap<QString, PMetaClassTable*> QueriesMap; //�������

	QVector<PMetaClassResource*> Resources; //������
	QVector<PMetaClassIndex*> IndexesCompound; //��������� �������

	QString CurrentXSN; //������� �������������� XSN
	QString CurrentXSR; //������� �������������� XSR
	QString CurrentXSF; //������� �������������� XSF
	bool Initialized;

	ISAssociationTypes AssociationTypes;
};
//-----------------------------------------------------------------------------
#endif
