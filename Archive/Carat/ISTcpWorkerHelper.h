#pragma once
#ifndef _ISTCPWORKERHELPER_H_INCLUDED
#define _ISTCPWORKERHELPER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISTcpWorkerHelper
{
public:
	static QString ConvertDateTimeToString(const QDateTime &DateTime, const QString &TimeFormat); //Конвертировать дату и время в строку
	static QString ConvertDateToString(const QDate &Date); //Конвертировать дату в строку
	static QString GetUptime(); //Получить Uptime сервера
	static QVariant GetSettingDB(const QString &DBConnectionName, const QString &SettingName); //Получить значение настройки БД
	static QString CreateSqlFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName); //Создание SQL-запроса для получения имени эскортной ссылки
	static QString CreateSqlFromTable(PMetaTable *MetaTable, QVariantMap &FilterMap, const QVariantList &SearchList, QString SortingField, Qt::SortOrder SortingOrder); //Создание SQL-запроса для таблиц
};
//-----------------------------------------------------------------------------
#endif
