#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISQueryModel : public QObject
{
	Q_OBJECT

public:
	ISQueryModel(PMetaTable *meta_table, ISNamespace::QueryModelType model_type, QObject *parent = 0);
	virtual ~ISQueryModel();

	void SetCondition(const QVariantMap &VariantMap);
	void AddCondition(const QString &Condition, const QVariant &Value);
	void ClearConditions();
	QVariantMap GetConditions();

	QString GetQueryText(); //�������� ����� ����� �������

	void SetParentFilter(unsigned int ParentObjectID, const QString &FieldName); //�������� ������� (�������������) ������� ��� ��������� �������

	void SetClassFilter(const QString &class_filter); //�������� ������
	QString GetClassFilter() const; //�������� ������
	void ClearClassFilter(); //�������� ������

	void SetSearchFilter(const QString &search_filter);
	QString GetSearchFilter() const;
	void ClearSearchFilter();

	QString GetSortingField() const;
	Qt::SortOrder GetSortingOrder() const;
	void SetSorting(const QString &FieldName, Qt::SortOrder Order); //�������� ��� �������, �� �������� ���� ����������

	void SetLimit(int limit); //�������� ���������� ��������� ������� �� ��������
	void SetOffset(int offset); //�������� ��������

private:
	QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator); //�������� ����� ������� ��� ������������� ���� ��������� ������� (������� � ����� ForeignViewNameField)
	QString GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator); //�������� ����� ��� ������� � ������ LEFT JOIN

private:
	PMetaTable *MetaTable;
	ISNamespace::QueryModelType ModelType;
	
	QString QuerySelectFields;
	QString QuerySelectFrom; //��������� � �������
	QString QuerySelectLeftJoin;
	QString QueryWhereText; //����� ������� �������
	
	QString SortingField; //��� ���� �� �������� ���� ����������
	Qt::SortOrder SortingOrder; //��� ����������
	bool SortingIsForeign; //����������� ���� �������� ������� ������
	bool SortingIsVirtual; //����������� ���� �������� �����������
	int Limit; //����� ������� �� ��������
	int Offset; //��������

	QString ClassFilter;
	QString SearchFilter;

	QVariantMap WhereMap;
	ISStringMap ForeignFields;
};
//-----------------------------------------------------------------------------
