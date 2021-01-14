#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
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
		: PMetaBase("Index"),
		Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

	QString GetName() //�������� ��� �������
	{
		QString IndexName;
		if (!Fields.empty()) //���� ������ ���������
		{
			IndexName += TableName + '_';
			for (const QString &String : Fields)
			{
				IndexName += String + '_';
			}
			IndexName.chop(1);
		}
		else
		{
			IndexName = TableName + '_' + FieldName;
		}
		return IndexName.toLower();
	}

	bool Unique;
	QString Alias;
	QString TableName;
	QString FieldName;
	ISVectorString Fields;
};
//-----------------------------------------------------------------------------
struct PMetaForeign : public PMetaBase
{
	PMetaForeign(const QString &field, const QString &foreign_class, const QString &foreign_field, const QString &foreign_view_name_field, const QString &table_name = QString())
		: PMetaBase("Foreign"),
		Field(field), ForeignClass(foreign_class), ForeignField(foreign_field), ForeignViewNameField(foreign_view_name_field), TableName(table_name) { }

	QString GetName() const //�������� ��� �������� �����
	{
		return (TableName + '_' + Field).toLower();
	}

	QString Field; //����, �� ������� ��������������� ������� ����
	QString ForeignClass; //�� ����� ������� ���������� ������� ����
	QString ForeignField; //�� ����� ���� ��������� ������� ����
	QString ForeignViewNameField; //����� ���� (����) ���������� � ������� �� �������

	QString TableName; //�������, ���������� ����, �� ������� ��������������� ������� ����
};
//-----------------------------------------------------------------------------
struct PMetaEscort : public PMetaBase
{
	PMetaEscort() : PMetaBase("Escort") { }

	QString LocalName;
	QString TableName;
	QString FilterField;
	QString ClassName;
};
//-----------------------------------------------------------------------------
struct PMetaField : public PMetaBase
{
	PMetaField() : PMetaBase("Field"),
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
        Foreign(nullptr),
        Sequence(false),
		PrimaryKey(false)
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
	bool PrimaryKey; //��������� ����
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
		for (PMetaField *MetaField : Fields)
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
		if (!Fields.empty())
		{
			return Fields[Index];
		}
		return nullptr;
	}

	PMetaField* GetFieldID() //�������� ���� "���"
	{
		if (!Fields.empty())
		{
			return Fields.front();
		}
		return nullptr;
	}

	int GetFieldIndex(const QString &FieldName) const //�������� ������ ���� �� ��� �����
	{
		for (size_t i = 0, c = Fields.size(); i < c; ++i)
		{
			PMetaField *MetaField = Fields[i];
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return (int)i;
			}
		}
		return -1;
	}

	bool ContainsField(const QString &FieldName)
	{
		for (PMetaField *MetaField : Fields)
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
	QString ObjectForm; //������������ ������ ����� �������
	bool ShowOnly; //������ �������� �������
	bool IsSystem;

	std::vector<PMetaEscort*> Escorts; //��������� �������
	std::vector<PMetaField*> Fields; //����
	std::vector<PMetaIndex*> IndexesCompound; //��������� ������� �������
};
//-----------------------------------------------------------------------------
struct PMetaFunction : public PMetaBase
{
	PMetaFunction() : PMetaBase("Function") { }

	QString Name; //��� �������
	QString Comment; //����������� �������
	QString Text; //����� �������
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
