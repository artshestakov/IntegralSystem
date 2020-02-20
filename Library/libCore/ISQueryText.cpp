#include "ISQueryText.h"
#include "ISAssert.h"
#include "ISDebug.h"
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
ISQueryText& ISQueryText::GetInstance()
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
	return true;
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

			if (Result)
			{
				for (int i = 0; i < ISMetaData::GetInstanse().GetMetaQueries().count(); ++i)
				{
					QString QueryName = ISMetaData::GetInstanse().GetMetaQueries().at(i);
					ISMetaViewQuery MetaViewQuery(QueryName);
					QString SqlText = MetaViewQuery.GetQueryText();

					QSqlQuery SqlQuery(ISDatabase::GetInstance().GetDefaultDB());
					SqlQuery.prepare(SqlText);
					if (SqlQuery.lastError().type() != QSqlError::NoError)
					{
						//ErrorQuery("Meta query: " + QueryName, SqlText, SqlQuery.lastError().text());
					}
				}
			}
			Vector.clear();
		}
		else
		{
			ErrorString = "Database not open connection";
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISQueryText::ErrorQuery(ISSqlQuery SqlQuery, const QString &ErrorText)
{
	QFile File(DEFINES_CORE.PATH_TEMP_DIR + ISSystem::GenerateUuid());
	if (File.open(QIODevice::WriteOnly))
	{
		QString Content;
		Content += "File: " + SqlQuery.FileName.toUtf8() + "\n";
		Content += "Line: " + QString::number(SqlQuery.Line).toUtf8() + "\n\n";
		Content += ErrorText;
		File.write(Content.toUtf8());
		File.close();

		if (DEFINES_CORE.IS_GUI)
		{
			QProcess::startDetached(DEFINES_CORE.PATH_APPLICATION_DIR + "/ErrorViewer.exe", QStringList() << File.fileName());
		}
		else
		{
			printf("%s\n", Content.toStdString().c_str());
		}
	}
}
//-----------------------------------------------------------------------------
