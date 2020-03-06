#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
#include "ISNamespace.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
struct PMetaClass
{
	PMetaClass(const QString &type_object) : TypeObject(type_object) { }

	QString TypeObject; //��� �������
};
//-----------------------------------------------------------------------------
struct PMetaClassIndex : public PMetaClass
{
	PMetaClassIndex() : PMetaClass("Index") { }
	PMetaClassIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name) : PMetaClass("Index"), Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

	bool Unique;
	QString Alias;
	QString TableName;
	QString FieldName;
	ISVectorString Fields;
};
//-----------------------------------------------------------------------------
struct PMetaClassForeign : public PMetaClass
{
	PMetaClassForeign() : PMetaClass("Foreign") { }

	QString Field; //����, �� ������� ��������������� ������� ����
	QString ForeignClass; //�� ����� ������� ���������� ������� ����
	QString ForeignField; //�� ����� ���� ���������� ������� ����
	QString ForeignViewNameField; //����� ���� (����) ���������� � ������� �� �������
	QString OrderField; //���� �� �������� ����� ����������� ����������

	QString TableName; //�������, ���������� ����, �� ������� ��������������� ������� ����
	QString SqlQuery; //������ �� ������ �� �������� �����
};
//-----------------------------------------------------------------------------
struct PMetaClassEscort : public PMetaClass
{
	PMetaClassEscort() : PMetaClass("Escort") { }

	QString LocalName;
	QString TableName;
	QString ClassName;
	QString ClassFilter;
};
//-----------------------------------------------------------------------------
struct PMetaClassField : public PMetaClass
{
	PMetaClassField() : PMetaClass("Field"),
		Sequence(false),
		Type(ISNamespace::FT_Unknown),
		Size(0),
		Upper(false),
		Lower(false),
		NotNull(false),
		ReadOnly(false),
		HideFromObject(false),
		HideFromList(false),
		NotSearch(false),
		IsSystem(false),
		Index(nullptr),
		Foreign(nullptr)
	{

	}

	bool IsFieldID() const { return Name.toLower() == "id"; }
	bool IsFieldUID() const { return Name.toLower() == "uid"; }

	ISUuid UID;
	ISNamespace::FieldType Type; //���
	QString Name; //��������
	int Size; //������
	bool Upper; //������ ������� �������
	bool Lower; //������ ������ �������
	QVariant DefaultValue; //�������� �� ��������� ��� ����
	QVariant DefaultValueWidget; //�������� �� ��������� ��� �������
	QString LabelName; //��� ���� �� ����� �������
	QString LocalListName; //��� ���� � ������
	bool NotNull; //���� �� ������ ���� ������
	bool ReadOnly; //�������������� ���� ���������
	bool HideFromObject; //���� ������ ���� ������ � ����� �������
	bool HideFromList; //���� �� ������ ������������ � �������
	bool NotSearch; //������� ���� � ������
	QString Hint; //��������� ��� ����
	QString PlaceholderText; //Placeholder ����� ��� �������
	QString ControlWidget; //������������ �������-��������� ��������
	QString RegExp; //���������� ��������� ��� ����
	bool IsSystem; //������ ���������� ����

	QString QueryText; //����� ����������

	PMetaClassIndex *Index; //������
	PMetaClassForeign *Foreign; //������� ����

	bool Sequence; //������������������ ����
	QString LayoutName; //������������ ������������ (��� ��������������� ���������� ����)
	QString SeparatorName; //������������ �������
};
//-----------------------------------------------------------------------------
struct PMetaClassTable : public PMetaClass
{
	PMetaClassTable() : PMetaClass("Table"), UseRoles(true), ShowOnly(false), IsSystem(false) { }
	PMetaClassTable(const QString &type_object) : PMetaClass(type_object), UseRoles(true), ShowOnly(false), IsSystem(false) { }

	PMetaClassField* GetField(const QString &FieldName) //�������� ���� �� �����
	{
		for (PMetaClassField *MetaField : AllFields)
		{
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return MetaField;
			}
		}
		return nullptr;
	}

	PMetaClassField* GetField(int Index) //�������� ���� �� �������
	{
		if (!AllFields.isEmpty())
		{
			return AllFields[Index];
		}
		return nullptr;
	}

	PMetaClassField* GetFieldID() //�������� ���� "���"
	{
		if (!SystemFields.isEmpty())
		{
			return SystemFields.front();
		}
		return nullptr;
	}

	int GetFieldIndex(const QString &FieldName) const //�������� ������ ���� �� ��� �����
	{
		for (int i = 0; i < AllFields.count(); ++i)
		{
			PMetaClassField *MetaField = AllFields.at(i);
			if (MetaField->Name == FieldName)
			{
				return i;
			}
		}
		return -1;
	}

	QString Name; //�������� �������
	ISUuid UID; //�������������
	QString Alias; //���������
	QString LocalName; //��������� ��� (� ������������ �����)
	QString LocalListName; //��������� ��� (� ������������� �����)
	QString TitleName;
	bool UseRoles;
	QString ClassFilter; //������ �������
	QString ClassFilterField;
	QString ObjectForm; //������������ ������ ����� �������
	bool ShowOnly; //������ �������� �������
	bool IsSystem;

	QString SqlModel; //������������ ������ ������

	QString Parent;
	QString Where;
	QString OrderField;

	QVector<PMetaClassEscort*> Escorts; //��������� �������
	QVector<PMetaClassField*> Fields; //����
	QVector<PMetaClassField*> SystemFields; //��������� ����
	QVector<PMetaClassField*> AllFields; //��� ����

	QVector<PMetaClassField*> SystemFieldsVisible; //������������ ��������� ����
	ISVectorString Joins;
};
//-----------------------------------------------------------------------------
struct PMetaClassFunction : public PMetaClass
{
	PMetaClassFunction() : PMetaClass("Function") { }

	QString Text;
};
//-----------------------------------------------------------------------------
struct PMetaClassQuery : public PMetaClassTable
{
public:
	PMetaClassQuery() : PMetaClassTable("Query") { }

	QString From;
	QString Where;
	QString Order;
	QString OrderType;
	ISVectorString Joins;
};
//-----------------------------------------------------------------------------
struct PMetaClassResource
{
	void AddField(const QString &FieldName, const QString &Value) //�������� �������� � ��� �������� � ������
	{
		if (FieldName.toLower() != "uid")
		{
			Parameters.emplace(FieldName, Value);
		}
	}

	QString TableName;
	ISUuid UID;
	ISStringMap Parameters;
};
//-----------------------------------------------------------------------------
#endif
