#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
struct PMetaBase
{
    PMetaBase(const std::string &type_object)
        : TypeObject(type_object.empty() ? "Unknown" : type_object) { }

    std::string TypeObject; //��� �������
};
//-----------------------------------------------------------------------------
struct PMetaIndex : public PMetaBase
{
    PMetaIndex(bool unique, const std::string &alias, const std::string &table_name, const std::string &field_name)
        : PMetaBase("Index"),
        Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

    std::string GetName() //�������� ��� �������
    {
        std::string IndexName;
        if (!Fields.empty()) //���� ������ ���������
        {
            IndexName += TableName + '_';
            for (const std::string &String : Fields)
            {
                IndexName += String + '_';
            }
            ISAlgorithm::StringChop(IndexName, 1);
        }
        else
        {
            IndexName = TableName + '_' + FieldName;
        }
        return ISAlgorithm::StringToLowerGet(IndexName);
    }

    bool Unique;
    std::string Alias;
    std::string TableName;
    std::string FieldName;
    ISVectorString Fields;
};
//-----------------------------------------------------------------------------
struct PMetaForeign : public PMetaBase
{
    PMetaForeign(const std::string &field, const std::string &foreign_class, const std::string &foreign_field, const std::string &foreign_view_name_field, const std::string &table_name = std::string())
        : PMetaBase("Foreign"),
        Field(field), ForeignClass(foreign_class), ForeignField(foreign_field), ForeignViewNameField(foreign_view_name_field), TableName(table_name) { }

    std::string GetName() const //�������� ��� �������� �����
    {
        return ISAlgorithm::StringToLowerGet(TableName + '_' + Field);
    }

    std::string Field; //����, �� ������� ��������������� ������� ����
    std::string ForeignClass; //�� ����� ������� ���������� ������� ����
    std::string ForeignField; //�� ����� ���� ��������� ������� ����
    std::string ForeignViewNameField; //����� ���� (����) ���������� � ������� �� �������

    std::string TableName; //�������, ���������� ����, �� ������� ��������������� ������� ����
};
//-----------------------------------------------------------------------------
struct PMetaField : public PMetaBase
{
    PMetaField() : PMetaBase("Field"),
        Type(ISNamespace::FieldType::Unknown),
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

    bool IsFieldID() const
    {
        return Type == ISNamespace::FieldType::ID;
    }

    std::string UID;
    ISNamespace::FieldType Type; //���
    std::string Name; //��������
    int Size; //������
    bool Upper; //������ ������� �������
    bool Lower; //������ ������ �������
    std::string DefaultValue; //�������� �� ��������� ��� ����
    std::string DefaultValueWidget; //�������� �� ��������� ��� �������
    std::string LabelName; //��� ���� �� ����� �������
    std::string LocalListName; //��� ���� � ������
    bool NotNull; //���� �� ������ ���� ������
    bool ReadOnly; //�������������� ���� ���������
    bool HideFromObject; //���� ������ ���� ������ � ����� �������
    bool HideFromList; //���� �� ������ ������������ � �������
    bool NotSearch; //������� ���� � ������
    std::string Hint; //��������� ��� ����
    std::string PlaceholderText; //Placeholder ����� ��� �������
    std::string ControlWidget; //������������ �������-��������� ��������
    std::string RegExp; //���������� ��������� ��� ����
    bool IsSystem; //������ ���������� ����

    std::string QueryText; //����� ����������

    PMetaIndex *Index; //������
    PMetaForeign *Foreign; //������� ����

    bool Sequence; //������������������ ����
    bool PrimaryKey; //��������� ����
    std::string LayoutName; //������������ ������������ (��� ��������������� ���������� ����)
    std::string SeparatorName; //������������ �������
};
//-----------------------------------------------------------------------------
struct PMetaEscort : public PMetaBase
{
    PMetaEscort() : PMetaBase("Escort") { }

    std::string LocalName;
    std::string TableName;
    std::string FilterField;
    std::string ClassName;
};
//-----------------------------------------------------------------------------
struct PMetaTable : public PMetaBase
{
    PMetaTable()
        : PMetaBase("Table"),
        IsSystem(false),
        ShowOnly(false)
    {

    }

    bool Exist(const std::string &FieldName)
    {
        for (PMetaField *MetaField : Fields)
        {
            if (MetaField->Name == FieldName)
            {
                return true;
            }
        }
        return false;
    }

    PMetaField* GetField(const std::string &FieldName)
    {
        for (PMetaField *MetaField : Fields)
        {
            if (ISAlgorithm::StringToLowerGet(MetaField->Name) == ISAlgorithm::StringToLowerGet(FieldName))
            {
                return MetaField;
            }
        }
        return nullptr;
    }

    std::string Name;
    std::string UID;
    std::string Alias;
    std::string LocalName;
    std::string LocalListName;
    std::string TitleName;
    bool IsSystem;
    bool ShowOnly;

    std::vector<PMetaField*> Fields;
    std::vector<PMetaIndex*> IndexesCompound; //��������� ������� �������
    std::vector<PMetaEscort*> Escorts; //��������� �������
};
//-----------------------------------------------------------------------------
struct PMetaFunction : public PMetaBase
{
    PMetaFunction(const std::string &name, const std::string &comment, const std::string &text)
        : PMetaBase("Function"),
        Name(name), Comment(comment), Text(text) { }

    std::string Name; //��� �������
    std::string Comment; //����������� �������
    std::string Text; //����� �������
};
//-----------------------------------------------------------------------------
struct PMetaResource : public PMetaBase
{
    PMetaResource() : PMetaBase("Resource") { }

    void AddField(const std::string &FieldName, const std::string &Value) //�������� �������� � ��� �������� � ������
    {
        std::string Temp = FieldName;
        ISAlgorithm::StringToLower(Temp);
        if (Temp != "uid")
        {
            Parameters.emplace(FieldName, Value);
        }
    }

    std::string TableName;
    std::string UID;
    ISMapString Parameters;
};
//-----------------------------------------------------------------------------
#endif
