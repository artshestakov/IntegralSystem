#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISQueryModelHelper
{
public:
	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator); //�������� ����� ������� ��� ������������� ���� ��������� ������� (������� � ����� ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator); //�������� ����� ��� ������� � ������ LEFT JOIN
};
//-----------------------------------------------------------------------------
