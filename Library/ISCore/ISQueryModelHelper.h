#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISQueryModelHelper
{
public:
	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, int Iterator); //�������� ����� ������� ��� ������������� ���� ��������� ������� (������� � ����� ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator); //�������� ����� ��� ������� � ������ LEFT JOIN
};
//-----------------------------------------------------------------------------
