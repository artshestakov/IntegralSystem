#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISQueryModel : public QObject
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

	void SetParentFilter(int ParentObjectID, const QString &FieldName); //�������� ������� (�������������) ������� ��� ��������� �������
	
	ISNamespace::PeriodType GetPeriodType() const;
	QDate GetPeriodBegin() const;
	QDate GetPeriodEnd() const;
	void SetPeriod(ISNamespace::PeriodType period_type, const QDate &period_begin, const QDate &period_end); //���������� ������
	void ClearPeriod(); //�������� ������

	void SetClassFilter(const QString &class_filter); //�������� ������
	QString GetClassFilter() const; //�������� ������
	void ClearClassFilter(); //�������� ������

	void SetSearchFilter(const QString &search_filter);
	QString GetSearchFilter() const;
	void ClearSearchFilter();

	void SetOrderField(const QString &FullFieldName, const QString &FieldName, Qt::SortOrder Order); //�������� ��� �������, �� �������� ���� ����������
	void SetLimit(int limit); //�������� ���������� ��������� ������� �� ��������
	void SetOffset(int offset); //�������� ��������

private:
	QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator); //�������� ����� ������� ��� ������������� ���� ��������� ������� (������� � ����� ForeignViewNameField)
	QString GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator); //�������� ����� ��� ������� � ������ LEFT JOIN

private:
	PMetaTable *MetaTable;
	ISNamespace::QueryModelType ModelType;
	QString ClassAlias;
	
	QString QuerySelectFields;
	QString QuerySelectFrom; //��������� � �������
	QString QuerySelectLeftJoin;
	QString QueryWhereText; //����� ������� �������
	
	QString QueryOrderText; //����������
	QString OrderField; //��� ���� �� �������� ���� ����������
	QString OrderFieldDefault; //���� �� ��������� �� ��� ����������
	Qt::SortOrder OrderSort; //��� ����������
	int Limit; //����� ������� �� ��������
	int Offset; //��������

	ISNamespace::PeriodType PeriodType;
	QDate PeriodBegin;
	QDate PeriodEnd;

	QString ClassFilter;
	QString SearchFilter;

	QVariantMap Conditions;
	ISStringMap ForeignFields;
};
//-----------------------------------------------------------------------------
