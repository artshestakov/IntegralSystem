#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
#include "ISNamespace.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
struct PMetaBase
{
	PMetaBase(const QString &type_object) : TypeObject(type_object.isEmpty() ? "Unknown" : type_object) { }

	QString TypeObject; //��� �������
};
//-----------------------------------------------------------------------------
struct PMetaIndex : public PMetaBase
{
	PMetaIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name) 
		: PMetaBase("Index"), Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

	bool Unique;
	QString Alias;
	QString TableName;
	QString FieldName;
	ISVectorString Fields;
};
//-----------------------------------------------------------------------------
struct PMetaForeign : public PMetaBase
{
	PMetaForeign() : PMetaBase("Foreign") { }

	QString Field; //����, �� ������� ��������������� ������� ����
	QString ForeignClass; //�� ����� ������� ���������� ������� ����
	QString ForeignField; //�� ����� ���� ��������� ������� ����
	QString ForeignViewNameField; //����� ���� (����) ���������� � ������� �� �������
	QString OrderField; //���� �� �������� ����� ����������� ����������

	QString TableName; //�������, ���������� ����, �� ������� ��������������� ������� ����
	QString SqlQuery; //������ �� ������ �� �������� �����
};
//-----------------------------------------------------------------------------
struct PMetaEscort : public PMetaBase
{
	PMetaEscort() : PMetaBase("Escort") { }

	QString LocalName;
	QString TableName;
	QString ClassName;
	QString ClassFilter;
};
//-----------------------------------------------------------------------------
struct PMetaField : public PMetaBase
{
	PMetaField() : PMetaBase("Field"),
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

	PMetaIndex *Index; //������
	PMetaForeign *Foreign; //������� ����

	bool Sequence; //������������������ ����
	QString LayoutName; //������������ ������������ (��� ��������������� ���������� ����)
	QString SeparatorName; //������������ �������
};
//-----------------------------------------------------------------------------
struct PMetaTable : public PMetaBase
{
	PMetaTable() : PMetaBase("Table"), UseRoles(true), ShowOnly(false), IsSystem(false) { }
	PMetaTable(const QString &type_object) : PMetaBase(type_object), UseRoles(true), ShowOnly(false), IsSystem(false) { }

	PMetaField* GetField(const QString &FieldName) //�������� ���� �� �����
	{
		for (PMetaField *MetaField : AllFields)
		{
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return MetaField;
			}
		}
		return nullptr;
	}

	PMetaField* GetField(int Index) //�������� ���� �� �������
	{
		if (!AllFields.empty())
		{
			return AllFields[Index];
		}
		return nullptr;
	}

	PMetaField* GetFieldID() //�������� ���� "���"
	{
		if (!SystemFields.empty())
		{
			return SystemFields.front();
		}
		return nullptr;
	}

	int GetFieldIndex(const QString &FieldName) const //�������� ������ ���� �� ��� �����
	{
		for (int i = 0, c = AllFields.size(); i < c; ++i)
		{
			PMetaField *MetaField = AllFields[i];
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return i;
			}
		}
		return -1;
	}

	bool ContainsField(const QString &FieldName)
	{
		for (PMetaField *MetaField : AllFields)
		{
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return true;
			}
		}
		return false;
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

	std::vector<PMetaEscort*> Escorts; //��������� �������
	std::vector<PMetaField*> Fields; //����
	std::vector<PMetaField*> SystemFields; //��������� ����
	std::vector<PMetaField*> AllFields; //��� ����

	std::vector<PMetaField*> SystemFieldsVisible; //������������ ��������� ����
	ISVectorString Joins;
};
//-----------------------------------------------------------------------------
struct PMetaFunction : public PMetaBase
{
	PMetaFunction() : PMetaBase("Function") { }

	QString Name; //��� �������
	QString Text; //����� �������
};
//-----------------------------------------------------------------------------
struct PMetaQuery : public PMetaTable
{
	PMetaQuery() : PMetaTable("Query") { }

	QString From;
	QString Where;
	QString Order;
	QString OrderType;
	ISVectorString Joins;
};
//-----------------------------------------------------------------------------
struct PMetaResource : public PMetaBase
{
	PMetaResource() : PMetaBase("Resource") { }

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
