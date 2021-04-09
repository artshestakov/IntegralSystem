#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
struct PMetaBase
{
    PMetaBase(const std::string &type_object)
        : TypeObject(type_object.empty() ? "Unknown" : type_object) { }

    std::string TypeObject; //��� �������
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
        //Index(nullptr),
        //Foreign(nullptr),
        Sequence(false),
        PrimaryKey(false)
    {

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

    //PMetaIndex *Index; //������
    //PMetaForeign *Foreign; //������� ����

    bool Sequence; //������������������ ����
    bool PrimaryKey; //��������� ����
    std::string LayoutName; //������������ ������������ (��� ��������������� ���������� ����)
    std::string SeparatorName; //������������ �������
};
//-----------------------------------------------------------------------------
struct PMetaTable : public PMetaBase
{
    PMetaTable() : PMetaBase("Table")
    {

    }

    std::string TableName;
    std::string UID;
    std::string Alias;
    std::string LocalName;
    std::string LocalListName;

    std::vector<PMetaField*> Fields;
};
//-----------------------------------------------------------------------------
#endif
