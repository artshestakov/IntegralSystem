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
	static QString GetColorForLogMessage(const QString &Severity); //Получить цвет для типа лог-сообщения
	static QString GetUptime(); //Получить Uptime сервера
	static QVariant GetSettingDB(const QString &DBConnectionName, const QString &SettingName); //Получить значение настройки БД
	static QString GenerateSqlQueryFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName); //Генерация SQL-запроса для получения имени эскортной ссылки
};
//-----------------------------------------------------------------------------
#endif
