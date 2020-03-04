#include "ISQuery.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISCountingTime.h"
#include "ISQueryExceptionConnection.h"
#include "ISQueryExceptionSyntax.h"
#include "ISQueryExceptionTransaction.h"
#include "ISAssert.h"
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
bool ISQuery::Prepare(const QString& text)
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
bool ISQuery::Prepare(QSqlDatabase& db, const QString& text)
{
	SqlText = text;
	SqlQuery = QSqlQuery(db);
	DB = db;

	return Prepare(text);
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute()
{
	ColumnIndices.clear();
	ISCountingTime Time;
	bool Result = SqlQuery.exec();
	int Msec = Time.GetElapsed();

	if (ShowLongQuery)
	{
		if (Msec > MAX_QUERY_TIME)
		{
			ISLOGGER_WARNING(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
		}
	}

	if (!Result)
	{
		Raise();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute(const QString& query_text)
{
	SqlText = query_text;
	ColumnIndices.clear();
	ISCountingTime Time;
	bool Result = SqlQuery.exec(query_text);
	int Msec = Time.GetElapsed();

	if (ShowLongQuery)
	{
		if (Msec > MAX_QUERY_TIME)
		{
			ISLOGGER_WARNING(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
		}
	}

	if (!Result)
	{
		Raise();
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute(QSqlDatabase& db, const QString& query_text)
{
	SqlText = query_text;
	ColumnIndices.clear();
	ISCountingTime Time;
	SqlQuery = db.exec(query_text);
	int Msec = Time.GetElapsed();

	if (ShowLongQuery)
	{
		if (Msec > MAX_QUERY_TIME)
		{
			ISLOGGER_WARNING(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
		}
	}
	Raise();
	return SqlQuery.lastError().type() == QSqlError::NoError;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst()
{
	bool Result = Execute();
	Result = Result && First();
	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst(QSqlDatabase& db, const QString& query_text)
{
	bool Result = Execute(db, query_text);
	Result == Result && First();
	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::BindValue(const QString& name, const QVariant& value)
{
	if (!SqlQuery.boundValues().contains(name))
	{
		IS_ASSERT(false, QString("Parameter \"%1\" not found in sql-query: %2").arg(name).arg(SqlText));
	}
	SqlQuery.bindValue(name, value);
	return true;
}
//-----------------------------------------------------------------------------
QVariant ISQuery::ReadColumn(const QString &name)
{
	if (ColumnIndices.empty())
	{
		PrepareColumnIndices();
	}

	std::map<QString, int>::const_iterator i = ColumnIndices.find(name.toLower());
	if (i == ColumnIndices.end())
	{
		IS_ASSERT(false, QString("Column \"%1\" not found in sql-query: %2").arg(name).arg(SqlText));
	}
	return ReadColumn(i->second);
}
//-----------------------------------------------------------------------------
QVariant ISQuery::ReadColumn(int index)
{
	return SqlQuery.value(index);
}
//-----------------------------------------------------------------------------
bool ISQuery::Next()
{
	bool Result = SqlQuery.next();
	if (!Result)
	{
		Raise();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::First()
{
	bool Result = SqlQuery.first();
	if (!Result)
	{
		Raise();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Previous()
{
	bool Result = SqlQuery.previous();
	if (!Result)
	{
		Raise();
	}
	return Result;
}
//-----------------------------------------------------------------------------
QSqlRecord ISQuery::GetRecord()
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
const std::map<QString, int>& ISQuery::GetColumnIndices()
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
void ISQuery::Raise()
{
	if (SqlQuery.lastError().type() != QSqlError::NoError)
	{
		ISLOGGER_ERROR(GetErrorText());
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
void ISQuery::PrepareColumnIndices()
{
	ColumnIndices.clear();
	QSqlRecord SqlRecord = SqlQuery.record();
	for (int i = 0, c = SqlRecord.count(); i < c; ++i)
	{
		ColumnIndices.emplace(SqlRecord.fieldName(i).toLower(), i);
	}
}
//-----------------------------------------------------------------------------
