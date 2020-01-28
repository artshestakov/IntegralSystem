#include "ISQueryText.h"
#include "ISAssert.h"
#include "ISDefines.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISMetaData.h"
#include "ISMetaViewQuery.h"
#include "ISDatabase.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISQueryText::ISQueryText()
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
QString ISQueryText::InsertQuery(const char *QueryText, const char *FileName, int Line)
{
	Queries.insert(QString("File: %1\nLine: %2").arg(QFileInfo(FileName).fileName()).arg(Line), QueryText);
	return QueryText;
}
//-----------------------------------------------------------------------------
void ISQueryText::CheckAllQueries()
{
	return;
	ISDebug::ShowDebugString("Checking queries");

	if (!Queries.count())
	{
		return;
	}

	IS_ASSERT(ISDatabase::GetInstance().GetDefaultDB().isOpen(), "Database not connected!");
	ISCountingTime Time;

    for (const auto &MapItem : Queries.toStdMap())
	{
		QSqlQuery SqlQuery(ISDatabase::GetInstance().GetDefaultDB());
		SqlQuery.prepare(MapItem.second);
		if (SqlQuery.lastError().type() != QSqlError::NoError)
		{
			ErrorQuery(MapItem.first, MapItem.second, SqlQuery.lastError().text());
		}
	}

	for (int i = 0; i < ISMetaData::GetInstanse().GetMetaQueries().count(); ++i)
	{
		QString QueryName = ISMetaData::GetInstanse().GetMetaQueries().at(i);
		ISMetaViewQuery MetaViewQuery(QueryName);
		QString SqlText = MetaViewQuery.GetQueryText();

		QSqlQuery SqlQuery(ISDatabase::GetInstance().GetDefaultDB());
		SqlQuery.prepare(SqlText);
		if (SqlQuery.lastError().type() != QSqlError::NoError)
		{
			ErrorQuery("Meta query: " + QueryName, SqlText, SqlQuery.lastError().text());
		}
	}

	ISDebug::ShowDebugString(QString("Check %1 queries time: %2").arg(Queries.count()).arg(ISSystem::MillisecondsToString(Time.GetElapsed())));
	Queries.clear();
}
//-----------------------------------------------------------------------------
void ISQueryText::ErrorQuery(const QString &About, const QString &SqlText, const QString &ErrorText)
{
	//???
	/*if (ISSystem::GetApplicationType() == ISNamespace::AT_GUI)
	{
		QMessageBox MessageBox;
		MessageBox.setWindowTitle(LOCALIZATION("Error"));
		MessageBox.setIcon(QMessageBox::Critical);
		MessageBox.setText(LOCALIZATION("Message.Warning.SqlQueryError"));
		MessageBox.setInformativeText(About + "\r\n\r\n" + ErrorText);
		MessageBox.setDetailedText(SqlText);
		MessageBox.exec();
	}*/
}
//-----------------------------------------------------------------------------
