#pragma once
#ifndef _ISQUERY_H_INCLUDED
#define _ISQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQuery
{
public:
    ISQuery(const QString &sql_text = QString(), bool prepare = true);
	ISQuery(QSqlDatabase &sql_database, const QString &sql_text = QString(), bool prepare = true);
	~ISQuery();

	bool Prepare(const QString &sql_text);
	bool Prepare(QSqlDatabase &sql_database, const QString &sql_text);

	bool Execute();
	bool Execute(const QString &sql_text);
	bool Execute(QSqlDatabase &sql_database, const QString &sql_text);

	bool ExecuteFirst();
	bool ExecuteFirst(QSqlDatabase &sql_database, const QString &sql_text);

	bool BindValue(const QString &ParameterName, const QVariant &ParameterValue);

	QVariant ReadColumn(const QString &ColumnName);
	QVariant ReadColumn(int ColumnIndex);
	
	bool Next();
	bool First();

	QSqlRecord GetRecord();
	QSqlQuery GetSqlQuery() const;

	bool IsValid() const;
	bool IsSelect() const;
	bool ExistParameter(const QString &ParameterName) const;
	QString GetSqlText() const; //�������� ����� �������
	QString GetErrorString(); //�������� ����� ������
	int GetErrorNumber() const; //�������� ����� ������
	int GetCountResultRows() const; //�������� ���������� ����� �� ���������� ������� (������ ��� SELECT)
	int GetCountAffected() const; //�������� ���������� ���������� �������� �����
	void SetShowLongQuery(bool show_long_query);

private:
	void Raise();
	void PrepareColumnIndices() throw();

private:
	QString ErrorString;
	QSqlQuery SqlQuery;
	QString SqlText;
	std::map<QString, int> ColumnIndices;
	QSqlDatabase SqlDatabase;
	bool Prepared;
	bool ShowLongQuery;
};
//-----------------------------------------------------------------------------
#endif
