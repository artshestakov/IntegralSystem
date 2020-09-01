#pragma once
#ifndef _CGDATABASE_H_INCLUDED
#define _CGDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGDatabase
{
public: //������� �����
	static bool Foreign_Create(PMetaForeign *MetaForeign, QString &ErrorString); //�������� �������� �����
	static bool Foreign_Update(PMetaForeign *MetaForeign, QString &ErrorString); //���������� �������� �����
	static bool Foreign_Exist(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString); //�������� ������������� �������� �����
	static QString Foreign_GetName(PMetaForeign *MetaForeign); //��������� ����� �������� �����

public: //�������
	static bool Function_CreateOrReplace(PMetaFunction *MetaFunction, QString &ErrorString); //�������� �������

public: //�������
	static bool Index_Create(PMetaIndex *Index, QString &ErrorString); //�������� �������
	static bool Index_Update(PMetaIndex *Index, QString &ErrorString); //���������� �������
	static bool Index_Exist(PMetaIndex *Index, bool &Exist, QString &ErrorString); //�������� ������������� �������
	static bool Index_CheckForeign(PMetaIndex *Index); //��������� ��������� �� �� ���� ������ ������� ����
	static bool Index_ReIndex(PMetaIndex *Index, QString &ErrorString); //����������� ������

public: //�������
	static bool Resource_Insert(PMetaResource *MetaResource, QString &ErrorString);
	static bool Resource_Update(PMetaResource *MetaResource, QString &ErrorString);
	static bool Resource_Exist(PMetaResource *MetaResource, bool &Exist, QString &ErrorString);
	static bool Resource_ResetField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������
	static bool Resource_UpdateField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������

public: //������������������
	static bool Sequence_Create(const QString &TableName, QString &ErrorString); //�������� ������������������
	static bool Sequence_Exist(const QString &TableName, bool &Exist, QString &ErrorString); //�������� ������������� ������������������
	static QString Sequence_GetName(const QString &TableName); //��������� ����� ������������������

public: //�������
	static bool Table_Create(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_Update(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_Exist(PMetaTable *MetaTable, bool &Exist, QString &ErrorString);
	static bool Table_AlterFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_CreateFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_DeleteFields(PMetaTable *MetaTable, QString &ErrorString);

public: //��������� �������
	static bool Helper_ExistField(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //��������� ������������� ���� � ������� ����
	static bool Helper_CommentTable(PMetaTable *MetaTable, QString &ErrorString); //�������� �������� � �������
	static bool Helper_CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString); //�������� �������� � ����
};
//-----------------------------------------------------------------------------
#endif
