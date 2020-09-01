#pragma once
#ifndef _CGDATABASE_H_INCLUDED
#define _CGDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGDatabase
{
public: //foreigns
	static bool CreateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //�������� �������� �����
	static bool UpdateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //���������� �������� �����
	static bool CheckExistForeign(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString); //�������� ������������� �������� �����
	static QString GetForeignName(PMetaForeign *MetaForeign); //��������� ����� �������� �����

public: //functions
	static bool CreateOrReplaceFunction(PMetaFunction *MetaFunction, QString &ErrorString); //�������� �������

public: //helper
	static bool CheckExistColumn(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //��������� ������������� ���� � ������� ����
	static bool CommentTable(PMetaTable *MetaTable, QString &ErrorString); //�������� �������� � �������
	static bool CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString); //�������� �������� � ����

public: //indexes
	static bool CreateIndex(PMetaIndex *Index, QString &ErrorString); //�������� �������
	static bool UpdateIndex(PMetaIndex *Index, QString &ErrorString); //���������� �������
	static bool CheckExistIndex(PMetaIndex *Index, bool &Exist, QString &ErrorString); //�������� ������������� �������
	static bool CheckIndexForeign(PMetaIndex *Index); //��������� ��������� �� �� ����� ������ ������� ����
	static bool ReindexIndex(PMetaIndex *Index, QString &ErrorString); //����������� ������

public: //resources
	static bool InsertResource(PMetaResource *MetaResource, QString &ErrorString);
	static bool UpdateResource(PMetaResource *MetaResource, QString &ErrorString);
	static bool CheckExistResource(PMetaResource *MetaResource, bool &Exist, QString &ErrorString);
	static bool ResetResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������
	static bool UpdateResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������

public: //sequence
	static bool CreateSequence(const QString &TableName, QString &ErrorString); //�������� ������������������
	static bool CheckExistSequence(const QString &TableName, bool &Exist, QString &ErrorString); //�������� ������������� ������������������
	static QString GetSequenceNameForTable(const QString &TableName); //��������� ����� ������������������

public: //tables
	static bool CreateTable(PMetaTable *MetaTable, QString &ErrorString);
	static bool UpdateTable(PMetaTable *MetaTable, QString &ErrorString);
	static bool CheckExistTable(PMetaTable *MetaTable, bool &Exist, QString &ErrorString);
	static bool AlterExistFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool CreateNewFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool DeleteOldFields(PMetaTable *MetaTable, QString &ErrorString);
};
//-----------------------------------------------------------------------------
#endif
