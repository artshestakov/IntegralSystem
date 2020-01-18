#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISNamespace.h"
#include "ISTypes.h"
#include "ISQueryModelHelper.h"
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQueryModel
{
public:
	ISQueryModel(PMetaClassTable *meta_table, ISNamespace::QueryModelType model_type);
	virtual ~ISQueryModel();

	void SetCondition(const QVariantMap &VariantMap);
	void AddCondition(const QString &Condition, const QVariant &Value);
	void ClearConditions();
	QVariantMap GetConditions();

	QString GetQueryText(); //�������� ����� ����� �������

	bool GetVisibleIsDeleted() const; //������������ �� ������ ���������� �� ��������
	void SetVisibleIsDeleted(bool Visible); //����������/�������� ��������� ������ ���������� �� ��������

	void SetParentObjectIDClassFilter(int ParentObjectID); //�������� ������� (�������������) ������� ��� ��������� �������
	
	ISNamespace::PeriodType GetPeriodType() const;
	ISRangeStruct GetPeriod() const;
	void SetPeriod(ISNamespace::PeriodType period_type, const ISRangeStruct &period_range); //���������� ������
	void ClearPeriod(); //�������� ������

	void SetClassFilter(const QString &class_filter); //�������� ������
	QString GetClassFilter() const; //�������� ������
	void ClearClassFilter(); //�������� ������

	void SetSearchFilter(const QString &search_filter);
	QString GetSearchFilter() const;
	void ClearSearchFilter();

	void SetOrderField(const QString &FullFieldName, const QString &FieldName); //�������� ��� �������, �� �������� ���� ����������
	void SetOrderSort(Qt::SortOrder order_sort); //�������� ��� ����������
	void SetLimit(int limit); //�������� ���������� ��������� ������� �� ��������
	void SetOffset(int offset); //�������� ��������

protected:
	void CreateQuerySelectSystemFields(); //�������� ������ ��������� ����� ��� ������� �������
	void CreateQuerySelectFields(); //�������� ������ ����� ��� ������� �������
	void CreateQuerySelectIsDeleted(); //�������� ������� ����������� ����� ���������/�� ���������
	void CheckQuery(const QString &QueryText); //�������� �������

private:
	PMetaClassTable *MetaTable;
	ISNamespace::QueryModelType ModelType;
	
	QString QuerySelectText; //����� ������� �� �������
	QString QuerySelectSystemFields; //������ ��������� ����� ��� ������� �� �������
	QString QuerySelectFields;
	QString QuerySelectFrom; //��������� � �������
	QString QuerySelectLeftJoin;
	QString QuerySelectIsDeleted;
	QString QueryWhereText; //����� ������� �������
	
	QString QueryOrderText; //����������
	QString OrderField; //��� ���� �� �������� ���� ����������
	QString OrderFieldDefault; //���� �� ��������� �� ��� ����������
	Qt::SortOrder OrderSort; //��� ����������
	int Limit; //����� ������� �� ��������
	int Offset; //��������

	bool VisibleIsDeleted;

	ISNamespace::PeriodType PeriodType;
	ISRangeStruct PeriodRange;

	QString ClassAlias;
	QString ClassFilter;
	QString SearchFilter;

	QVariantMap Conditions;
	QStringMap ForeignFields;
};
//-----------------------------------------------------------------------------
