#include "ISQuery.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISDatabase.h"
#include "ISCountingTime.h"
#include "ISQueryExceptionConnection.h"
#include "ISQueryExceptionSyntax.h"
#include "ISQueryExceptionTransaction.h"
//-----------------------------------------------------------------------------
ISQuery::ISQuery(const QString& text, bool prepare)
	: SqlQuery(ISDatabase::GetInstance().GetDefaultDB()),
	DB(ISDatabase::GetInstance().GetDefaultDB()),
	SqlText(text),
	Prepared(false),
	ShowLongQuery(true)
{
	if (prepare && !text.isEmpty())
	{
		Prepare(text);
	}
	else if (!prepare && !text.isEmpty())
	{
		Execute(text);
	}
}
//-----------------------------------------------------------------------------
ISQuery::ISQuery(QSqlDatabase& db, const QString& text, bool prepare)
	: SqlQuery(db),
	DB(db),
	SqlText(text),
	Prepared(false)
{
	if (prepare && !text.isEmpty())
	{
		Prepare(text);
	}
	else if (!prepare &&!text.isEmpty())
	{
		Execute(text);
	}
}
//-----------------------------------------------------------------------------
ISQuery::~ISQuery()
{
	
}
//-----------------------------------------------------------------------------
bool ISQuery::Prepare(const QString& text) throw(ISQueryException)
{
	SqlText = text;
	Prepared = SqlQuery.prepare(text);
	if (!Prepared)
	{
		Raise();
	}

	return Prepared;
}
//-----------------------------------------------------------------------------
bool ISQuery::Prepare(QSqlDatabase& db, const QString& text) throw(ISQueryException)
{
	SqlText = text;
	SqlQuery = QSqlQuery(db);
	DB = db;

	return Prepare(text);
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute() throw(ISQueryException)
{
	ColumnIndices.clear();
	ISCountingTime Time;
	bool Result = SqlQuery.exec();
	int Msec = Time.GetElapsed();

	if (Msec > MAX_QUERY_TIME)
	{
		if (ShowLongQuery)
		{
			ISDebug::ShowWarningString(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
		}
	}

	if (!Result)
	{
		Raise();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute(const QString& query_text) throw(ISQueryException)
{
	SqlText = query_text;
	ColumnIndices.clear();
	ISCountingTime Time;
	bool Result = SqlQuery.exec(query_text);
	int Msec = Time.GetElapsed();

	if (Msec > MAX_QUERY_TIME)
	{
		if (ShowLongQuery)
		{
			ISDebug::ShowWarningString(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
		}
	}

	if (!Result)
	{
		Raise();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute(QSqlDatabase& db, const QString& query_text) throw(ISQueryException)
{
	SqlText = query_text;
	ColumnIndices.clear();
	ISCountingTime Time;
	SqlQuery = db.exec(query_text);
	int Msec = Time.GetElapsed();

	if (Msec > MAX_QUERY_TIME)
	{
		if (ShowLongQuery)
		{
			ISDebug::ShowWarningString(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
		}
	}

	Raise();

	return SqlQuery.lastError().type() == QSqlError::NoError;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst() throw(ISQueryException)
{
	bool ok = Execute();
	ok = ok && First();
	return ok;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst(QSqlDatabase& db, const QString& query_text) throw(ISQueryException)
{
	bool ok = Execute(db, query_text);
	ok == ok && First();
	return ok;
}
//-----------------------------------------------------------------------------
bool ISQuery::BindValue(const QString& name, const QVariant& value) throw(ISQueryException)
{
	if (!SqlQuery.boundValues().contains(name))
	{
		throw ISQueryExceptionSyntax(QString("Parameter \"%1\" not found in sql-query: %2").arg(name).arg(SqlText));
		return false;
	}

	SqlQuery.bindValue(name, value);
	ParametersQuery.insert(name, value);
	return true;
}
//-----------------------------------------------------------------------------
QVariant ISQuery::ReadColumn(const QString &name) throw(ISQueryException)
{
	if (ColumnIndices.empty())
	{
		PrepareColumnIndices();
	}

	QMap<QString, int>::iterator i = ColumnIndices.find(name.toLower());
	if (i != ColumnIndices.end())
	{
		return ReadColumn(*i);
	}

	throw(ISQueryExceptionSyntax(QString("Column \"%1\" not found in sql-query: %2").arg(name).arg(SqlText)));
	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISQuery::ReadColumn(int index) throw(ISQueryException)
{
	return SqlQuery.value(index);
}
//-----------------------------------------------------------------------------
bool ISQuery::Next() throw(ISQueryException)
{
	bool Result = SqlQuery.next();
	if (!Result)
	{
		Raise();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::First() throw(ISQueryException)
{
	bool Result = SqlQuery.first();
	if (!Result)
	{
		Raise();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Previous() throw(ISQueryException)
{
	bool Result = SqlQuery.previous();
	if (!Result)
	{
		Raise();
	}

	return Result;
}
//-----------------------------------------------------------------------------
QSqlRecord ISQuery::GetRecord() throw(ISQueryException)
{
	if (SqlQuery.lastError().type() == QSqlError::NoError)
	{
		return SqlQuery.record();
	}
	else 
	{
		Raise();
	}

	return QSqlRecord();
}
//-----------------------------------------------------------------------------
QSqlQuery ISQuery::GetSqlQuery() const
{
	return SqlQuery;
}
//-----------------------------------------------------------------------------
bool ISQuery::IsValid() const
{
	return SqlQuery.isValid();
}
//-----------------------------------------------------------------------------
bool ISQuery::IsSelect() const
{
	return SqlQuery.isSelect();
}
//-----------------------------------------------------------------------------
bool ISQuery::ExistParameter(const QString &Name) const
{
	return SqlQuery.boundValues().count(Name);
}
//-----------------------------------------------------------------------------
QString ISQuery::GetSqlText() const
{
	return SqlText;
}
//-----------------------------------------------------------------------------
QString ISQuery::GetErrorText()
{
	return SqlQuery.lastError().text();
}
//-----------------------------------------------------------------------------
int ISQuery::GetErrorNumber() const
{
	return SqlQuery.lastError().number();
}
//-----------------------------------------------------------------------------
const QMap<QString, int>& ISQuery::GetColumnIndices()
{
	if (ColumnIndices.empty())
	{
		PrepareColumnIndices();
	}

	return ColumnIndices;
}
//-----------------------------------------------------------------------------
int ISQuery::GetCountResultRows() const
{
	IS_ASSERT(SqlQuery.isSelect(), "Query not select");
	return SqlQuery.size();
}
//-----------------------------------------------------------------------------
int ISQuery::GetCountAffected() const
{
	return SqlQuery.numRowsAffected();
}
//-----------------------------------------------------------------------------
void ISQuery::SetShowLongQuery(bool show)
{
	ShowLongQuery = show;
}
//-----------------------------------------------------------------------------
void ISQuery::Raise() throw(ISQueryException)
{
	if (SqlQuery.lastError().type() != QSqlError::NoError)
	{
		ISDebug::ShowCriticalString(GetErrorText());
	}

	if (!DB.isOpen())
	{
		throw ISQueryExceptionConnection(SqlQuery.lastError().text());
	}

	switch (SqlQuery.lastError().type())
	{
	case QSqlError::NoError: return; break;
	case QSqlError::StatementError: throw ISQueryExceptionSyntax(SqlQuery.lastError().text()); break;
	case QSqlError::ConnectionError: throw ISQueryExceptionConnection(SqlQuery.lastError().text()); break;
	case QSqlError::TransactionError: throw ISQueryExceptionTransaction(SqlQuery.lastError().text()); break;
	}
}
//-----------------------------------------------------------------------------
void ISQuery::PrepareColumnIndices() throw()
{
	ColumnIndices.clear();
	QSqlRecord SqlRecord = SqlQuery.record();
	for (int i = 0; i < SqlRecord.count(); ++i)
	{
		ColumnIndices.insert(SqlRecord.fieldName(i).toLower(), i);
	}
}
//-----------------------------------------------------------------------------
