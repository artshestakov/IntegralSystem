#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQueryModelHelper
{
public:
	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaClassForeign *MetaForeign, int Iterator); //�������� ����� ������� ��� ������������� ���� ��������� ������� (������� � ����� ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator); //�������� ����� ��� ������� � ������ LEFT JOIN
};
//-----------------------------------------------------------------------------
