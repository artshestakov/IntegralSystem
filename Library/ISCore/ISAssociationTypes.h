#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISAssociationTypes
{
public:
	ISAssociationTypes();
	~ISAssociationTypes();

	void Insert(const QString &type_name, ISNamespace::FieldType meta_field_type, const QString &database_type, const QString &control_widget, const QString &search_operator_widget, bool search); //�������� ���������� ����

	ISNamespace::FieldType GetTypeField(const QString &type_name); //�������� ��� ������ �� ����-�����
	QString GetTypeDB(ISNamespace::FieldType field_type) const; //�������� ������������ ���� � ���� ������
	QString GetTypeDB(const QString &type_name) const; //�������� ������������ ���� � ���� ������
	QString GetControlWidgetFromType(ISNamespace::FieldType field_type) const; //�������� ������������ ������ ��� ���� ������
	QString GetSearchOperatorWidget(ISNamespace::FieldType field_type) const; //�������� ������������ ������ ��������� ������
	bool GetSearch(ISNamespace::FieldType field_type) const; //�������� ���������� �� ����� �� ���� ������

private:
	QVector<ISMetaDataType> Types;
};
//-----------------------------------------------------------------------------
