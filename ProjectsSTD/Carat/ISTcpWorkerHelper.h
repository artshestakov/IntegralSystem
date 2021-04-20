#pragma once
#ifndef _ISTCPWORKERHELPER_H_INCLUDED
#define _ISTCPWORKERHELPER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISDateTime.h"
//-----------------------------------------------------------------------------
class ISTcpWorkerHelper
{
public:
	static std::string ConvertDateTimeToString(const ISDateTime &DateTime); //�������������� ���� � ����� � ������
	static std::string ConvertDateToString(const ISDate &Date); //�������������� ���� � ������
	static std::string GetUptime(); //�������� Uptime �������
	//static QVariant GetSettingDB(const std::string &DBConnectionName, const std::string &SettingName); //�������� �������� ��������� ��
	//static std::string CreateSqlFromTitleName(PMetaForeign *MetaForeign, const std::string &Alias, const std::string &FieldName); //�������� SQL-������� ��� ��������� ����� ��������� ������
	//static std::string CreateSqlFromTable(PMetaTable *MetaTable, QVariantMap &FilterMap, const QVariantList &SearchList, std::string SortingField, ISNamespace::SortingOrder SortingOrder); //�������� SQL-������� ��� ������
};
//-----------------------------------------------------------------------------
#endif
