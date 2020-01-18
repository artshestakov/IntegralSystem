#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class ISQueryModelHelper
{
public:
	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaClassForeign *MetaForeign, int Iterator); //�������� ����� ������� ��� ������������� ���� ��������� ������� (������� � ����� ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator); //�������� ����� ��� ������� � ������ LEFT JOIN
};
//-----------------------------------------------------------------------------
