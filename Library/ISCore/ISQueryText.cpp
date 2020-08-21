#include "ISQueryText.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISMetaData.h"
#include "ISMetaViewQuery.h"
#include "ISDatabase.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
ISQueryText::ISQueryText()
	: ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISQueryText::~ISQueryText()
{

}
//-----------------------------------------------------------------------------
ISQueryText& ISQueryText::Instance()
{
	static ISQueryText QueryText;
	return QueryText;
}
//-----------------------------------------------------------------------------
QString ISQueryText::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
QString ISQueryText::InsertQuery(const char *QueryText, const char *FileName, int Line)
{
	Vector.emplace_back(ISSqlQuery{ FileName, Line, QueryText });
	return QueryText;
}
//-----------------------------------------------------------------------------
bool ISQueryText::CheckAllQueries()
{
	bool Result = !Vector.empty();
	if (Result)
	{
		Result = ISDatabase::Instance().GetDB(CONNECTION_DEFAULT).isOpen();
		if (Result)
		{
			QSqlQuery SqlQuery(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT));
			for (const ISSqlQuery &Item : Vector)
			{
				Result = SqlQuery.prepare(Item.SqlText);
				if (Result)
				{
					Result = SqlQuery.lastError().type() == QSqlError::NoError;
				}
				
				if (!Result)
				{
					ErrorQuery(Item, SqlQuery.lastError().text());
					break;
				}
			}
			Vector.clear();

			if (Result)
			{
				for (const QString &SqlText : ISMetaData::Instance().GetMetaQueries())
				{
					QSqlQuery SqlMetaQuery(ISDatabase::Instance().GetDB(CONNECTION_DEFAULT));
					Result = SqlMetaQuery.prepare(SqlText);
					if (Result)
					{
						Result = SqlMetaQuery.lastError().type() == QSqlError::NoError;
					}

					if (!Result)
					{
						ErrorQuery(ISSqlQuery{ "NULL", 0, SqlText }, "Error prepare meta query: " + SqlMetaQuery.lastError().text());
						break;
					}
				}
			}
		}
		else
		{
			ErrorString = "Database is not open";
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISQueryText::ErrorQuery(const ISSqlQuery &SqlQuery, const QString &error_string)
{
	ErrorString = QString("File: %1\nLine: %2\nSqlQuery: %3\n\n%4").arg(SqlQuery.FileName).arg(SqlQuery.Line).arg(SqlQuery.SqlText).arg(error_string);
	QFile File(ISDefines::Core::PATH_TEMP_DIR + "/" + ISSystem::GenerateUuid());
	if (File.open(QIODevice::WriteOnly))
	{
		File.write(ErrorString.toUtf8());
		File.close();
		if (!ISDefines::Core::IS_GUI)
		{
			printf("%s\n", ErrorString.toStdString().c_str());
		}
	}
}
//-----------------------------------------------------------------------------
