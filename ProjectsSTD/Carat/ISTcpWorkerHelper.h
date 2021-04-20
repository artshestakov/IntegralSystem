#pragma once
#ifndef _ISTCPWORKERHELPER_H_INCLUDED
#define _ISTCPWORKERHELPER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISDateTime.h"
//-----------------------------------------------------------------------------
class ISTcpWorkerHelper
{
public:
	static std::string ConvertDateTimeToString(const ISDateTime &DateTime); //Конвертировать дату и время в строку
	static std::string ConvertDateToString(const ISDate &Date); //Конвертировать дату в строку
	static std::string GetUptime(); //Получить Uptime сервера
	//static QVariant GetSettingDB(const std::string &DBConnectionName, const std::string &SettingName); //Получить значение настройки БД
	//static std::string CreateSqlFromTitleName(PMetaForeign *MetaForeign, const std::string &Alias, const std::string &FieldName); //Создание SQL-запроса для получения имени эскортной ссылки
	//static std::string CreateSqlFromTable(PMetaTable *MetaTable, QVariantMap &FilterMap, const QVariantList &SearchList, std::string SortingField, ISNamespace::SortingOrder SortingOrder); //Создание SQL-запроса для таблиц
};
//-----------------------------------------------------------------------------
#endif
