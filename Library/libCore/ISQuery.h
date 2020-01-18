#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQueryException.h"
#include "ISQueryExceptionConnection.h"
#include "ISQueryExceptionSyntax.h"
#include "ISQueryExceptionTransaction.h"
#include "ISQueryText.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISQuery
{
public:
    ISQuery(const QString& text = QString(), bool prepare = true);
	ISQuery(QSqlDatabase& db, const QString &text = QString(), bool prepare = true);
	virtual ~ISQuery();

	bool Prepare(const QString& text) throw(ISQueryException);
	bool Prepare(QSqlDatabase& db, const QString& text) throw(ISQueryException);

	bool Execute() throw(ISQueryException);
	bool Execute(const QString& query_text) throw(ISQueryException);
	bool Execute(QSqlDatabase& db, const QString& query_text) throw(ISQueryException);

	bool ExecuteFirst() throw(ISQueryException);
	bool ExecuteFirst(QSqlDatabase& db, const QString& query_text) throw(ISQueryException);

	bool BindValue(const QString& name, const QVariant& value) throw(ISQueryException);

	QVariant ReadColumn(const QString& name) throw(ISQueryException);
	QVariant ReadColumn(int index) throw(ISQueryException);
	
	bool Next() throw(ISQueryException);
	bool First() throw(ISQueryException);
	bool Previous() throw(ISQueryException);

	QSqlRecord GetRecord() throw(ISQueryException);
	QSqlQuery GetSqlQuery() const;

	bool IsValid() const;
	bool IsSelect() const;
	bool ExistParameter(const QString &Name) const;
	QString GetSqlText() const; //Получить текст запроса
	QString GetErrorText(); //Получить текст ошибки
	int GetErrorNumber() const; //Получить номер ошибки
	const QMap<QString, int>& GetColumnIndices();
	int GetCountResultRows() const; //Получить количество строк из результата запроса (только для SELECT)
	int GetCountAffected() const; //Получить количество затронутых запросом строк
	void SetShowLongQuery(bool show);

protected:
	void Raise() throw(ISQueryException);
	void PrepareColumnIndices() throw();

private:
	QSqlQuery SqlQuery;
	QMap<QString, int> ColumnIndices;
	QSqlDatabase DB;
	QString SqlText;
	bool Prepared;
	QVariantMap ParametersQuery;
	bool ShowLongQuery;
};
//-----------------------------------------------------------------------------
