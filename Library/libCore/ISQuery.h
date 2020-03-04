#pragma once
#ifndef _ISQUERY_H_INCLUDED
#define _ISQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISQueryText.h"
#include "ISQueryException.h"
#include "ISQueryExceptionSyntax.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQuery
{
public:
    ISQuery(const QString& text = QString(), bool prepare = true);
	ISQuery(QSqlDatabase& db, const QString &text = QString(), bool prepare = true);
	~ISQuery();

	bool Prepare(const QString& text);
	bool Prepare(QSqlDatabase& db, const QString& text);

	bool Execute();
	bool Execute(const QString& query_text);
	bool Execute(QSqlDatabase& db, const QString& query_text);

	bool ExecuteFirst();
	bool ExecuteFirst(QSqlDatabase& db, const QString& query_text);

	bool BindValue(const QString& name, const QVariant& value);

	QVariant ReadColumn(const QString& name);
	QVariant ReadColumn(int index);
	
	bool Next();
	bool First();
	bool Previous();

	QSqlRecord GetRecord();
	QSqlQuery GetSqlQuery() const;

	bool IsValid() const;
	bool IsSelect() const;
	bool ExistParameter(const QString &Name) const;
	QString GetSqlText() const; //�������� ����� �������
	QString GetErrorText(); //�������� ����� ������
	int GetErrorNumber() const; //�������� ����� ������
	const std::map<QString, int>& GetColumnIndices();
	int GetCountResultRows() const; //�������� ���������� ����� �� ���������� ������� (������ ��� SELECT)
	int GetCountAffected() const; //�������� ���������� ���������� �������� �����
	void SetShowLongQuery(bool show);

private:
	void Raise();
	void PrepareColumnIndices() throw();

private:
	QSqlQuery SqlQuery;
	std::map<QString, int> ColumnIndices;
	QSqlDatabase DB;
	QString SqlText;
	bool Prepared;
	bool ShowLongQuery;
};
//-----------------------------------------------------------------------------
#endif
