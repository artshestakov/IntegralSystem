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
	static QString ConvertDateTimeToString(const QDateTime &DateTime, const QString &TimeFormat); //�������������� ���� � ����� � ������
	static QString ConvertDateToString(const QDate &Date); //�������������� ���� � ������
	static QString GetColorForLogMessage(const QString &Severity); //�������� ���� ��� ���� ���-���������
	static QString GetUptime(); //�������� Uptime �������
	static QVariant GetSettingDB(const QString &DBConnectionName, const QString &SettingName); //�������� �������� ��������� ��
	static QString GenerateSqlQueryFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName); //��������� SQL-������� ��� ��������� ����� ��������� ������
};
//-----------------------------------------------------------------------------
#endif
