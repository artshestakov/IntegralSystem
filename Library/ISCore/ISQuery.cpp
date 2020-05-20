#include "ISQuery.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISCountingTime.h"
#include "ISAssert.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
ISQuery::ISQuery(const QString &sql_text, bool prepare)
    : ErrorString(NO_ERROR_STRING),
      SqlQuery(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT)),
      SqlText(sql_text),
      SqlDatabase(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT)),
      Prepared(false),
      ShowLongQuery(true)
{
    if (prepare && !sql_text.isEmpty())
    {
        Prepare(sql_text);
    }
    else if (!prepare && !sql_text.isEmpty())
    {
        Execute(sql_text);
    }
}
//-----------------------------------------------------------------------------
ISQuery::ISQuery(const QSqlDatabase &sql_database, const QString &sql_text, bool prepare)
    : ErrorString(NO_ERROR_STRING),
      SqlQuery(sql_database),
      SqlText(sql_text),
      SqlDatabase(sql_database),
      Prepared(false)
{
    if (prepare && !sql_text.isEmpty())
    {
        Prepare(sql_text);
    }
    else if (!prepare && !sql_text.isEmpty())
    {
        Execute(sql_text);
    }
}
//-----------------------------------------------------------------------------
ISQuery::~ISQuery()
{

}
//-----------------------------------------------------------------------------
bool ISQuery::Prepare(const QString &sql_text)
{
    SqlText = sql_text;
    Prepared = SqlQuery.prepare(sql_text);
    if (!Prepared)
    {
        ErrorString = SqlQuery.lastError().databaseText();
    }
    return Prepared;
}
//-----------------------------------------------------------------------------
bool ISQuery::Prepare(QSqlDatabase &sql_database, const QString &sql_text)
{
    SqlText = sql_text;
    SqlQuery = QSqlQuery(sql_database);
    SqlDatabase = sql_database;
    return Prepare(sql_text);
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute()
{
    ColumnIndices.clear();
    ISCountingTime Time;
    bool Result = SqlQuery.exec();
    unsigned int Msec = Time.Elapsed();
    if (ShowLongQuery)
    {
        if (Msec > MAX_QUERY_TIME)
        {
            ISLOGGER_W(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
        }
    }

    if (!Result)
    {
		ErrorString = SqlQuery.lastError().databaseText();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute(const QString &sql_text)
{
    SqlText = sql_text;
    ColumnIndices.clear();
    ISCountingTime Time;
    bool Result = SqlQuery.exec(sql_text);
    unsigned int Msec = Time.Elapsed();
    if (ShowLongQuery)
    {
        if (Msec > MAX_QUERY_TIME)
        {
            ISLOGGER_W(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
        }
    }

    if (!Result)
    {
		ErrorString = SqlQuery.lastError().databaseText();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::Execute(QSqlDatabase &sql_database, const QString &sql_text)
{
    SqlText = sql_text;
    ColumnIndices.clear();
    ISCountingTime Time;
    SqlQuery = sql_database.exec(sql_text);
    unsigned int Msec = Time.Elapsed();
    if (ShowLongQuery)
    {
        if (Msec > MAX_QUERY_TIME)
        {
            ISLOGGER_W(QString("Long query %1 msec: %2").arg(Msec).arg(SqlQuery.lastQuery().simplified()));
        }
    }
	bool Result = SqlQuery.lastError().type() == QSqlError::NoError;
	if (!Result)
	{
		ErrorString = SqlQuery.lastError().databaseText();
	}
    return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst()
{
    bool Result = Execute();
    if (Result)
    {
        Result = First();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::ExecuteFirst(QSqlDatabase &sql_database, const QString &sql_text)
{
    bool Result = Execute(sql_database, sql_text);
    if (Result)
    {
        Result = First();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::BindValue(const QString& name, const QVariant& value)
{
    IS_ASSERT(SqlQuery.boundValues().contains(name), QString("Parameter \"%1\" not found in sql-query: %2").arg(name).arg(SqlText));
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

    ISStringToIntMap::const_iterator Iterator = ColumnIndices.find(name.toLower());
    IS_ASSERT(Iterator != ColumnIndices.end(), QString("Column \"%1\" not found in sql-query: %2").arg(name).arg(SqlText));
    return ReadColumn(Iterator->second);
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
		ErrorString = SqlQuery.lastError().databaseText();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISQuery::First()
{
    bool Result = SqlQuery.first();
    if (!Result)
    {
		ErrorString = SqlQuery.lastError().databaseText();
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
		ErrorString = SqlQuery.lastError().databaseText();
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
QString ISQuery::GetErrorString()
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
int ISQuery::GetErrorNumber() const
{
    return SqlQuery.lastError().number();
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
void ISQuery::SetShowLongQuery(bool show_long_query)
{
    ShowLongQuery = show_long_query;
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
