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
		Result = ISDatabase::GetInstance().GetDefaultDB().isOpen();
		if (Result)
		{
			QSqlQuery SqlQuery(ISDatabase::GetInstance().GetDefaultDB());
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
				for (int i = 0; i < ISMetaData::GetInstanse().GetMetaQueries().count(); ++i)
				{
					QString QueryName = ISMetaData::GetInstanse().GetMetaQueries().at(i);
					QString SqlText = ISMetaViewQuery(QueryName).GetQueryText();

					QSqlQuery SqlQuery(ISDatabase::GetInstance().GetDefaultDB());
					Result = SqlQuery.prepare(SqlText);
					if (Result)
					{
						Result = SqlQuery.lastError().type() != QSqlError::NoError;
					}

					if (!Result)
					{
						//???
						ErrorString = SqlQuery.lastError().text();
						//ErrorQuery("Meta query: " + QueryName, SqlText, SqlQuery.lastError().text());
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
void ISQueryText::ErrorQuery(ISSqlQuery SqlQuery, const QString &ErrorText)
{
	ErrorString = ErrorText;
	QFile File(ISDefines::Core::PATH_TEMP_DIR + "/" + ISSystem::GenerateUuid());
	if (File.open(QIODevice::WriteOnly))
	{
		QString Content;
		Content += "File: " + SqlQuery.FileName.toUtf8() + "\n";
		Content += "Line: " + QString::number(SqlQuery.Line).toUtf8() + "\n\n";
		Content += ErrorText;
		File.write(Content.toUtf8());
		File.close();

		if (ISDefines::Core::IS_GUI)
		{
			QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_DIR + "/ErrorViewer.exe", QStringList() << File.fileName());
		}
		else
		{
			printf("%s\n", Content.toStdString().c_str());
		}
	}
}
//-----------------------------------------------------------------------------
