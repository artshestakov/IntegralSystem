#pragma once
#ifndef _ISTCPWORKERHELPER_H_INCLUDED
#define _ISTCPWORKERHELPER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISDateTime.h"
#include "PMetaClass.h"
#include "rapidjson/document.h"
#include "libpq-fe.h"
//-----------------------------------------------------------------------------
class ISTcpWorkerHelper
{
public:
	static std::string ConvertDateTimeToString(const ISDateTime &DateTime); //�������������� ���� � ����� � ������
	static std::string ConvertDateToString(const ISDate &Date); //�������������� ���� � ������
	static std::string GetUptime(); //�������� Uptime �������
	static const char* GetSettingDB(PGconn *DBConnection, const std::string &SettingName); //�������� �������� ��������� ��
	static std::string CreateSqlFromTitleName(PMetaForeign *MetaForeign, const std::string &Alias, const std::string &FieldName); //�������� SQL-������� ��� ��������� ����� ��������� ������
	static std::string CreateSqlFromTable(PMetaTable *MetaTable, rapidjson::Value &FilterObject, const rapidjson::Value &SearchArray, std::string SortingField, ISNamespace::SortingOrder SortingOrder); //�������� SQL-������� ��� ������
    static size_t WriteFunctionINN(void *Pointer, size_t Size, size_t NMemb, std::string *Data);
};
//-----------------------------------------------------------------------------
#endif
