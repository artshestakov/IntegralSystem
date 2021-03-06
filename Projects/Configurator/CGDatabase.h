#pragma once
#ifndef _CGDATABASE_H_INCLUDED
#define _CGDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGDatabase
{
public: //������� �����
    static bool Foreign_Create(PMetaForeign *MetaForeign, std::string &ErrorString); //�������� �������� �����
    static bool Foreign_Update(PMetaForeign *MetaForeign, std::string &ErrorString); //���������� �������� �����
    static bool Foreign_Exist(PMetaForeign *MetaForeign, bool &Exist, std::string &ErrorString); //�������� ������������� �������� �����

public: //�������
    static bool Function_Create(PMetaFunction *MetaFunction, std::string &ErrorString); //�������� �������
    static bool Function_Delete(std::string &ErrorString); //�������� ���������� �������

public: //�������
    static bool Index_Create(PMetaIndex *Index, std::string &ErrorString); //�������� �������
    static bool Index_Update(PMetaIndex *Index, std::string &ErrorString); //��������� ������
    static bool Index_Delete(PMetaIndex *Index, std::string &ErrorString); //������� ������
    static bool Index_Exist(PMetaIndex *Index, bool &Exist, std::string &ErrorString); //�������� ������������� �������
    //static bool Index_CheckForeign(PMetaIndex *Index); //��������� ��������� �� �� ���� ������ ������� ����
    //static bool Index_ReIndex(PMetaIndex *Index, QString &ErrorString); //����������� ������

public: //�������
    static bool Resource_Insert(PMetaResource *MetaResource, std::string &ErrorString);
    static bool Resource_Update(PMetaResource *MetaResource, std::string &ErrorString);
    static bool Resource_Exist(PMetaResource *MetaResource, bool &Exist, std::string &ErrorString);
    static bool Resource_ResetField(const std::string &TableName, const std::string &TableAlias, const std::string &FieldName, const std::string &ResourceUID, std::string &ErrorString); //�������� ���� ������� � �������
    static bool Resource_UpdateField(const std::string &TableName, const std::string &TableAlias, const std::string &FieldName, const std::string &Value, const std::string &ResourceUID, std::string &ErrorString); //�������� ���� ������� � �������

public: //������������������
    static bool Sequence_Create(const std::string &TableName, std::string &ErrorString); //�������� ������������������
    static bool Sequence_Exist(const std::string &TableName, bool &Exist, std::string &ErrorString); //�������� ������������� ������������������
    static std::string Sequence_GetName(const std::string &TableName); //��������� ����� ������������������

public: //�������
    static bool Table_Create(PMetaTable *MetaTable, std::string &ErrorString);
    static bool Table_Update(PMetaTable *MetaTable, std::string &ErrorString);
    static bool Table_Exist(PMetaTable *MetaTable, bool &Exist, std::string &ErrorString);
    static bool Table_AlterFields(PMetaTable *MetaTable, std::string &ErrorString);
    static bool Table_CreateFields(PMetaTable *MetaTable, std::string &ErrorString);
    static bool Table_DeleteFields(PMetaTable *MetaTable, std::string &ErrorString);

public: //��������� �������
    static bool Helper_ExistField(PMetaTable *MetaTable, const std::string &ColumnName, bool &Exist, std::string &ErrorString); //��������� ������������� ���� � ������� ����
    static bool Helper_CommentTable(PMetaTable *MetaTable, std::string &ErrorString); //�������� �������� � �������
    static bool Helper_CommentField(PMetaTable *MetaTable, PMetaField *MetaField, std::string &ErrorString); //�������� �������� � ����
    static bool Helper_CommentFunction(PMetaFunction *MetaFunction, std::string &ErrorString); //�������� �������� �������
};
//-----------------------------------------------------------------------------
#endif
